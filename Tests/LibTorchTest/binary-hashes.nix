version:
builtins.getAttr version {
  "2.6.0" = {
    aarch64-darwin-cpu = {
      name = "libtorch-macos-arm64-2.6.0.zip";
      url = "https://download.pytorch.org/libtorch/cpu/libtorch-macos-arm64-2.6.0.zip";
      hash = "sha256-4d9YKUuvAESBeG/WCUsQfEHwdB2z34grnlwWslj4970=";
    };
    x86_64-linux-cpu = {
      name = "libtorch-cxx11-abi-shared-with-deps+2.6.0-cpu.zip";
      url = "https://download.pytorch.org/libtorch/cpu/libtorch-cxx11-abi-shared-with-deps-2.6.0%2Bcpu.zip";
      hash = "sha256-gUzPhc4Z8rTPhIm89pPoLP0Ww17ono+/xgMW46E/Tro=";
    };
    x86_64-linux-cuda = {
      name = "libtorch-cxx11-abi-shared-with-deps+2.6.0-cu124.zip";
      url = "https://download.pytorch.org/libtorch/cu124/libtorch-cxx11-abi-shared-with-deps-2.6.0%2Bcu124.zip";
      hash = "sha256-rJIvNGcR/xFfkr/O2a32CmO5/5Fv24Y7+efOtYgGO6A=";
    };
    x86_64-linux-rocm = {
      name = "libtorch-cxx11-abi-shared-with-deps-2.6.0+rocm6.2.4.zip";
      url = "https://download.pytorch.org/libtorch/rocm6.2.4/libtorch-cxx11-abi-shared-with-deps-2.6.0%2Brocm6.2.4.zip";
      hash = "sha256-UaX47GAwyZ6UmzgY85TeAHmy3u52pBHhiyM5NAz7ens=";
    };
  };
}
