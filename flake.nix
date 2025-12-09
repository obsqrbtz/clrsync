{
  description = "clrsync - Color scheme manager";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs, ... }: let
    system = "x86_64-linux";
    pkgs = nixpkgs.legacyPackages.${system};
  in {
    packages.${system} = rec {
      clrsync-git = pkgs.stdenv.mkDerivation rec {
        pname = "clrsync-git";
        version = "unstable";

        src = pkgs.fetchgit {
          url = "https://github.com/obsqrbtz/clrsync.git";
          rev = "HEAD";
          sha256 = "sha256-znRtdLfdEmfdSGeiQA0v6hcZBFsQzgQtPY5uN98lwjo=";
        };

        nativeBuildInputs = [
          pkgs.cmake
          pkgs.git
          pkgs.pkg-config
          pkgs.makeWrapper
        ];

        buildInputs = [
          pkgs.glfw
          pkgs.freetype
          pkgs.fontconfig
          pkgs.xorg.libXcursor
          pkgs.mesa
          pkgs.xorg.libX11
          pkgs.xorg.libXrandr
          pkgs.xorg.libXi
          pkgs.xorg.libXinerama
          pkgs.xorg.libXcursor
          pkgs.wayland
          pkgs.wayland-protocols
          pkgs.libxkbcommon
          pkgs.zlib
          pkgs.bzip2
        ];

        cmakeFlags = [
          "-DCMAKE_BUILD_TYPE=Release"
          "-DUSE_SYSTEM_GLFW=ON"
        ];

        installPhase = ''
          cmake --install . --prefix $out
          wrapProgram $out/bin/clrsync_gui \
            --prefix LD_LIBRARY_PATH : ${pkgs.lib.makeLibraryPath buildInputs}
          wrapProgram $out/bin/clrsync_cli \
            --prefix LD_LIBRARY_PATH : ${pkgs.lib.makeLibraryPath buildInputs}
        '';

        meta = with pkgs.lib; {
          description = "Color scheme manager (git version)";
          homepage = "https://github.com/obsqrbtz/clrsync";
          license = licenses.mit;
          platforms = platforms.linux;
          mainProgram = "clrsync_gui";
        };
      };

      default = clrsync-git;
    };

    apps.${system} = {
      clrsync-gui = {
        type = "app";
        program = "${self.packages.${system}.clrsync-git}/bin/clrsync_gui";
      };
      
      clrsync-cli = {
        type = "app";
        program = "${self.packages.${system}.clrsync-git}/bin/clrsync_cli";
      };
      
      default = self.apps.${system}.clrsync-gui;
    };

    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [
        cmake
        git
        pkg-config
        glfw
        freetype
        fontconfig
        xorg.libXcursor
        mesa
      ];
    };
  };
}
