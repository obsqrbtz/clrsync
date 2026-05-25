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
  libpng,
  fontconfig,
  xxd,
  mesa,
  libglvnd,
  xorg,
  wayland,
  libxkbcommon,
  zlib,
  bzip2,
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
    xxd
  ];

  buildInputs = [
    glfw
    libglvnd
    freetype
    libpng
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
    bzip2
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

  postFixup = lib.optionalString stdenv.hostPlatform.isLinux ''
    if [ -f "$out/bin/.clrsync_gui-wrapped" ]; then
      wrapProgram "$out/bin/.clrsync_gui-wrapped" \
        --prefix LD_LIBRARY_PATH : "${lib.makeLibraryPath [ mesa.out libglvnd ]}" \
        --set-default GBM_BACKENDS_PATH "${mesa}/lib/gbm" \
        --set-default __EGL_VENDOR_LIBRARY_FILENAMES__ "${mesa}/share/glvnd/egl_vendor.d/50_mesa.json" \
        --prefix LIBGL_DRIVERS_PATH : "${mesa}/lib/dri"
    fi
  '';

  meta = with lib; {
    description = "Color scheme manager with GUI and CLI";
    homepage = "https://github.com/obsqrbtz/clrsync";
    license = licenses.mit;
    platforms = platforms.linux;
    mainProgram = "clrsync_gui";
    maintainers = [ "Daniel Dada" ];
  };
}
