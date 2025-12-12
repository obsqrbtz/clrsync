{
  description = "clrsync - Color scheme manager";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in
    {
      packages.${system} = rec {
        clrsync = pkgs.stdenv.mkDerivation rec {
          pname = "clrsync";
          version = "unstable";

          src = self;

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.git
            pkgs.pkg-config
            pkgs.makeWrapper
            pkgs.wayland-protocols
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

        default = clrsync;
      };

      homeManagerModules.default = import ./home-manager-module.nix;
      homeManagerModules.clrsync = self.homeManagerModules.default;

      apps.${system} = {
        clrsync-gui = {
          type = "app";
          program = "${self.packages.${system}.clrsync}/bin/clrsync_gui";
        };

        clrsync-cli = {
          type = "app";
          program = "${self.packages.${system}.clrsync}/bin/clrsync_cli";
        };

        default = self.apps.${system}.clrsync-gui;
      };

      devShells.${system}.default = pkgs.mkShell {
        buildInputs = with pkgs; [
          cmake
          git
          pkg-config
          makeWrapper
          ninja
          stdenv.cc.cc.lib
          clang-tools
          gdb

          glfw
          freetype
          fontconfig
          mesa
          xorg.libX11
          xorg.libXrandr
          xorg.libXi
          xorg.libXinerama
          xorg.libXcursor
          libxkbcommon
          zlib
          bzip2
          wayland
          wayland-scanner
          wayland-protocols
        ];

        shellHook = ''
          export CMAKE_GENERATOR="Ninja"
          export CMAKE_EXPORT_COMPILE_COMMANDS=1
          export CPLUS_INCLUDE_PATH="${pkgs.gcc.cc}/include/c++/${pkgs.gcc.cc.version}:${pkgs.gcc.cc}/include/c++/${pkgs.gcc.cc.version}/x86_64-unknown-linux-gnu:$CPLUS_INCLUDE_PATH"
          export C_INCLUDE_PATH="${pkgs.gcc.cc}/include:$C_INCLUDE_PATH"
          export PATH=${pkgs.wayland-protocols}/bin:$PATH
        '';
      };

    };
}
