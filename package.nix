{
  lib,
  stdenv,
  cmake,
  git,
  pkg-config,
  makeWrapper,
  wrapGAppsHook3,
  wayland-protocols,
  glfw,
  freetype,
  fontconfig,
  mesa,
  xorg,
  wayland,
  libxkbcommon,
  zlib,
  wayland-scanner,
  gtk3,
  glib,
  gsettings-desktop-schemas,
  semver,
}:

stdenv.mkDerivation rec {
  pname = "clrsync";

  version = semver;

  src = lib.cleanSourceWith {
    src = ./.;
    filter =
      path: type:
      let
        baseName = baseNameOf path;
      in
      !(
        lib.hasSuffix ".o" baseName
        || lib.hasSuffix ".a" baseName
        || baseName == "build"
        || baseName == "CMakeCache.txt"
        || baseName == "CMakeFiles"
        || baseName == ".git"
        || baseName == "result"
        || baseName == ".direnv"
      );
  };

  nativeBuildInputs = [
    cmake
    git
    pkg-config
    makeWrapper
    wrapGAppsHook3
    wayland-protocols
  ];

  buildInputs = [
    glfw
    freetype
    fontconfig
    xorg.libXcursor
    mesa
    xorg.libX11
    xorg.libXrandr
    xorg.libXi
    xorg.libXinerama
    wayland
    wayland-scanner
    wayland-protocols
    libxkbcommon
    zlib
    gtk3
    gsettings-desktop-schemas
    glib
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DUSE_SYSTEM_GLFW=ON"
    "-DCLRSYNC_SEMVER=${version}"
  ];

  installPhase = ''
    runHook preInstall

    cmake --install . --prefix $out

    runHook postInstall
  '';

  dontWrapGApps = false;

  meta = with lib; {
    description = "Color scheme manager with GUI and CLI";
    homepage = "https://github.com/obsqrbtz/clrsync";
    license = licenses.mit;
    platforms = platforms.linux;
    mainProgram = "clrsync_gui";
    maintainers = [ "Daniel Dada" ];
  };
}
