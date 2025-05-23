{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils = {
        url = "github:numtide/flake-utils";
        # inputs.system.follows = "systems";
    };
};

outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem
    (system:
    let
        pkgs = import nixpkgs {
            inherit system;
            config.allowUnfree = true;
        };
        libs = [
            pkgs.libcxx
            pkgs.hdf5
        ];
    in
        with pkgs;
            {
            devShells.default = mkShell {
                nativeBuildInputs = with pkgs; [
                    bashInteractive
                    (opencv.override ({
                        enableGtk3 = true;
                        enableCuda = false;
                        enableFfmpeg = true;
                        enableUnfree = true;
                    }))
                    cmake
                    libgcc
                    libcxx
                    pkg-config
                    gcc
                    hdf5
                ];
                buildInputs = with pkgs; [
                    python311Packages.h5py
                    python311Packages.matplotlib
                ];
                shellHook = ''
                '';
                    # ./compile.sh debug
                LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}";
                    # cmake --no-warn-unused-cli -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER:FILEPATH=`which gcc` -DCMAKE_INSTALL_PREFIX=install -DCMAKE_CXX_COMPILER:FILEPATH=`which g++` -S./ -B./build -G "Unix Makefiles"
            };
        }
    );
}
