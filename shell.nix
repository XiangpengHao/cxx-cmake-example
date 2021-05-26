let
  name = "nixos-20.09-2021-05-26";
  commit-id = "76ed24ceab9ec8b520f977a2803181f0c1d86b4d";
  pkgs-url = "https://github.com/nixos/nixpkgs/archive/${commit-id}.tar.gz";

  oxalica-rust-overlay = import (fetchTarball https://github.com/oxalica/rust-overlay/archive/master.tar.gz);

  pkgs = import (builtins.fetchTarball { url = pkgs-url; }) {
    overlays = [
      oxalica-rust-overlay
    ];
  };

  # ----- Rust --------------------------------------------------------------------
  extras = {
    extensions = [
      "rust-analysis" # Rust Language Server (RLS)
      "rust-src"      # Needed by RLS? or only rust-analyzer?
      "rls-preview"   # What do *-preview offer?
      "rustfmt-preview"
      "clippy-preview"
    ];
    #targets = [ "arg-unknown-linux-gnueabihf" ];
  };

  # If you already have a rust-toolchain file for rustup, you can simply use
  # fromRustupToolchainFile to get the customized toolchain derivation.
  rust-tcfile  = pkgs.rust-bin.fromRustupToolchainFile ./rust-toolchain;

  rust-latest  = pkgs.rust-bin.stable .latest      .default;
  rust-beta    = pkgs.rust-bin.beta   ."2021-05-25".default;
  rust-nightly = pkgs.rust-bin.nightly."2021-05-25".default;
  rust-stable  = pkgs.rust-bin.stable ."1.52.1"    .default;

  # Rust system to be used in buldiInputs. Choose between
  # latest/beta/nightly/stable on the next line
  rust = rust-stable.override extras;

  llvmPkgs = pkgs.buildPackages.llvmPackages_12;
  mkShell = pkgs.mkShell.override { stdenv = llvmPkgs.stdenv; };

# -----

in mkShell {
  inherit name;
  buildInputs = [
    llvmPkgs.lldClang.bintools
    rust
    pkgs.cmake
  ];
  hardeningDisable = [ "fortify" ];
  NIX_CFLAGS_LINK = "-fuse-ld=lld";
}
