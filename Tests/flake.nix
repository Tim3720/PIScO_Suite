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

<<<<<<< HEAD
        libtorch = pkgs.callPackage ./default.nix {};
=======
        custom_libtorch = pkgs.callPackage ./default.nix {};
>>>>>>> f824addef806546f229596685e2bb0d8a2d1ec85
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
<<<<<<< HEAD
                    gcc12
                ];
                buildInputs = with pkgs; [
                    (python312.withPackages(ps: with ps;[
                        # (opencv4.override {enableGtk2 = true;})
                        numpy
                        scipy
                        matplotlib
                    ]))
=======
                    cudatoolkit
                    cudaPackages.cuda_cudart
>>>>>>> f824addef806546f229596685e2bb0d8a2d1ec85
                ];
                buildInputs = [
                    (opencv.override {
                        enableGtk3 = true;
                        # enableCuda = false;
                        enableFfmpeg = true;
                        enableUnfree = true;
                    })
                    pkgs.python3Packages.torch
                    (pkgs.python3Packages.opencv4.override { enableGtk3 = true; })
                    custom_libtorch
                ] ;

                shellHook = ''
                    echo ${custom_libtorch.dev}
                    echo ${custom_libtorch.out}
                '';
                LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}";
            };
        }
    );
}
