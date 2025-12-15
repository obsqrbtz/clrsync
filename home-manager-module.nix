flake:
{
  config,
  lib,
  pkgs,
  ...
}:
with lib;
let
  cfg = config.programs.clrsync;

  clrsyncPackage = flake.packages.${pkgs.system}.default;

  templateType = types.submodule {
    options = {
      enabled = mkOption {
        type = types.bool;
        default = true;
        description = "Whether to enable this template.";
      };
      inputPath = mkOption {
        type = types.str;
        description = "Path to the template input file.";
      };
      outputPath = mkOption {
        type = types.str;
        description = "Path where the generated output will be written.";
      };
      reloadCmd = mkOption {
        type = types.str;
        default = "";
        description = "Command to run after generating the output.";
      };
    };
  };

  configFormat = pkgs.formats.toml { };
  configFile = configFormat.generate "config.toml" {
    general = {
      default_theme = cfg.defaultTheme;
      palettes_path = cfg.palettesPath;
      font = cfg.font;
      font_size = cfg.fontSize;
    };
    templates = mapAttrs (
      name: template: {
        enabled = template.enabled;
        input_path = template.inputPath;
        output_path = template.outputPath;
        reload_cmd = template.reloadCmd;
      }
    ) cfg.templates;
  };
in
{
  options.programs.clrsync = {
    enable = mkEnableOption "clrsync color synchronization";

    defaultTheme = mkOption {
      type = types.str;
      default = "cursed";
      description = "Default theme to use.";
    };

    palettesPath = mkOption {
      type = types.str;
      default = "~/.config/clrsync/palettes";
      description = "Path to color palettes directory.";
    };

    font = mkOption {
      type = types.str;
      default = "JetBrainsMono Nerd Font Mono";
      description = "Font family to use.";
    };

    fontSize = mkOption {
      type = types.int;
      default = 14;
      description = "Font size.";
    };

    templates = mkOption {
      type = types.attrsOf templateType;
      default = { };
      description = "Template configurations.";
      example = literalExpression ''
        {
          kitty = {
            enabled = true;
            inputPath = "~/.config/clrsync/templates/kitty.conf";
            outputPath = "~/.config/kitty/kitty_test.conf";
            reloadCmd = "pkill -SIGUSR1 kitty";
          };
        }
      '';
    };

    applyTheme = mkOption {
      type = types.bool;
      default = false;
      description = "Whether to apply the default theme on activation.";
    };

    systemdTarget = mkOption {
      type = types.str;
      default = "graphical-session.target";
      description = "Systemd target to bind the clrsync service to.";
    };
  };

  config = mkIf cfg.enable {
    home.packages = [ clrsyncPackage ];

      xdg.enable = true;

    home.activation.clrsyncDesktop = lib.hm.dag.entryAfter [ "writeBoundary" ] ''
      if [ -d "$HOME/.nix-profile/share/applications" ]; then
        ${pkgs.desktop-file-utils}/bin/update-desktop-database "$HOME/.nix-profile/share/applications" || true
      fi
    '';

    xdg.configFile."clrsync/config.toml" = {
      source = configFile;
      force = true;
    };

    home.activation.clrsyncConfig = lib.hm.dag.entryAfter [ "writeBoundary" ] ''
      run --quiet mkdir -p $HOME/.config/clrsync
      run --quiet cp -f ${configFile} $HOME/.config/clrsync/config.toml
    '';

    home.activation.clrsyncApply = mkIf cfg.applyTheme (
      lib.hm.dag.entryAfter [ "clrsyncConfig" ] ''
        run --quiet ${clrsyncPackage}/bin/clrsync_cli --apply --theme ${cfg.defaultTheme}
      ''
    );

    systemd.user.services.clrsync = mkIf cfg.applyTheme {
      Unit = {
        Description = "Apply clrsync color palette";
        After = [ cfg.systemdTarget ];
        PartOf = [ cfg.systemdTarget ];
      };
      Service = {
        Type = "oneshot";
        ExecStart = "${clrsyncPackage}/bin/clrsync_cli --apply --theme ${cfg.defaultTheme}";
        RemainAfterExit = true;
      };
      Install = {
        WantedBy = [ cfg.systemdTarget ];
      };
    };
  };
}