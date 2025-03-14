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

        custom_libtorch = pkgs.callPackage ./default.nix {};
    in
        with pkgs;
            {
            devShells.default = mkShell {
                nativeBuildInputs = with pkgs; [
                    bashInteractive
                    libcxx
                    pkg-config
                    cudatoolkit
                ];
                buildInputs = [
                    pkgs.python3Packages.torch
                    (pkgs.python3Packages.opencv4.override { enableGtk3 = true; })
                    custom_libtorch
                ] ;

                shellHook = ''
                '';
                    # echo ${pkgs.libtorch-bin}
                LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}";
            };
        }
    );
}
