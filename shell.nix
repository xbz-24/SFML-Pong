{ pkgs ? import <nixpkgs> {
    overlays = [
      (self: super: {
        libGL =
          let
            inherit (self) lib stdenv libglvnd testers;
            inherit (self.darwin.apple_sdk.frameworks) OpenGL;
          in
          stdenv.mkDerivation {
            pname = "libGL";
            version = if stdenv.hostPlatform.isDarwin then "4.1" else libglvnd.version;
            outputs = [ "out" "dev" ];

            buildCommand = if stdenv.hostPlatform.isDarwin then ''
              mkdir -p $out/nix-support $dev/lib/pkgconfig $dev/nix-support

              echo ${OpenGL} >> $out/nix-support/propagated-build-inputs
              echo "$out" > $dev/nix-support/propagated-build-inputs

              mkdir -p $dev/include
              ln -s ${OpenGL}/Library/Frameworks/OpenGL.framework/Headers $dev/include/GL

              cat <<EOF >$dev/lib/pkgconfig/gl.pc
              Name: gl
              Description: gl library
              Version: 4.1
              Libs: -F${OpenGL} -framework OpenGL
              Cflags: -F${OpenGL} -DAPIENTRY=GLAPIENTRY
              EOF
            '' else super.libGL.buildCommand;

            meta = {
              description = "Stub bindings using " + (if stdenv.hostPlatform.isDarwin then "OpenGL.framework" else "libglvnd");
              pkgConfigModules = [ "gl" ] ++ lib.optionals (!stdenv.hostPlatform.isDarwin) [ "egl" "glesv1_cm" "glesv2" ];
            } // (if stdenv.hostPlatform.isDarwin
              then { inherit (OpenGL.meta) homepage platforms; }
              else { inherit (libglvnd.meta) homepage license platforms; });
          };
      })
    ];
  }, lib ? pkgs.lib }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    sfml
    box2d
    clang
    pkg-config
  ];
}
