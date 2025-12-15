{
  description = "clrsync - Color scheme manager";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      supportedSystems = [
        "x86_64-linux"
      ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; });

      baseVersion = "0.1.4";

semver = 
  if self ? rev 
  then "${baseVersion}+${toString self.revCount}.git.${builtins.substring 0 7 self.rev}"
  else "${baseVersion}+dev";  # Accept that local builds show "+dev"
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};
        in
        rec {
          clrsync = pkgs.callPackage ./package.nix { inherit semver; };
          default = clrsync;
        }
      );

      homeModules = {
        default = import ./home-manager-module.nix self;
        clrsync = self.homeModules.default;
      };

      apps = forAllSystems (system: {
        clrsync-gui = {
          type = "app";
          program = "${self.packages.${system}.clrsync}/bin/clrsync_gui";
          meta = {
            description = "clrsync gui app";
            license = self.packages.x86_64-linux.licenses.mit;
            maintainers = [ "Daniel Dada" ];
          };
        };

        clrsync-cli = {
          type = "app";
          program = "${self.packages.${system}.clrsync}/bin/clrsync_cli";
          meta = {
            description = "clrsync cli app";
            license = self.packages.x86_64-linux.licenses.mit;
            maintainers = [ "Daniel Dada" ];
          };
        };

        default = self.apps.${system}.clrsync-cli;
      });

      devShells = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.clrsync ];

            buildInputs = with pkgs; [
              cmake
              ninja
              clang-tools
              gdb
            ];

            shellHook = ''
              export CMAKE_GENERATOR="Ninja"
              export CMAKE_EXPORT_COMPILE_COMMANDS=1
            '';
          };
        }
      );

      overlays.default = final: prev: {
        clrsync = self.packages.${final.system}.clrsync;
      };
    };
}
