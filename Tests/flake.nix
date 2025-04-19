{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
    flake-utils = {
        url = "github:numtide/flake-utils";
        inputs.system.follows = "systems";
    };
};

outputs = { self, nixpkgs, flake-utils, ... } @inputs:
    flake-utils.lib.eachDefaultSystem
    (system:
    let
        pkgs = import nixpkgs {
            inherit system;
            config.allowUnfree = true;
            config.cudaSupport = true;
        };
        libs = [
            pkgs.libcxx
        ];
    in
        with pkgs;
        {
            # packages.${system} = {
            #     libtorch = libtorch;
            # };
            # defaultPackage.${system} = libtorch;

            devShells.default = mkShell {
                default = libtorch;
                nativeBuildInputs = with pkgs; [
                    bashInteractive
                    libcxx
                    pkg-config
                ];
                buildInputs = with pkgs; [
                    (python312.withPackages(ps: with ps; [
                        matplotlib
                        numpy
                        (opencv4.override { enableGtk3 = true; })
                    ]))
                    # pkgs.python3Packages.torch
                    # (pkgs.python3Packages.)
                ] ;

                shellHook = ''
                '';
                LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}";
            };
        }
    );
}
