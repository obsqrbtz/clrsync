{
  lib,
  stdenv,
  cmake,
  git,
  pkg-config,
  makeWrapper,
  wayland-protocols,
  glfw,
  freetype,
  fontconfig,
  mesa,
  xorg,
  wayland,
  libxkbcommon,
  zlib,
  bzip2,
  wayland-scanner,
  semver
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
    bzip2
  ];

  cmakeFlags = [
    "-DCMAKE_BUILD_TYPE=Release"
    "-DUSE_SYSTEM_GLFW=ON"
    "-DCLRSYNC_SEMVER=${version}"
  ];

  installPhase = ''
    runHook preInstall

    cmake --install . --prefix $out

    wrapProgram $out/bin/clrsync_gui \
      --prefix LD_LIBRARY_PATH : ${lib.makeLibraryPath buildInputs}

    wrapProgram $out/bin/clrsync_cli \
      --prefix LD_LIBRARY_PATH : ${lib.makeLibraryPath buildInputs}

    runHook postInstall
  '';

  meta = with lib; {
    description = "Color scheme manager with GUI and CLI";
    homepage = "https://github.com/obsqrbtz/clrsync";
    license = licenses.mit;
    platforms = platforms.linux;
    mainProgram = "clrsync_gui";
    maintainers = [ ];
  };
}
