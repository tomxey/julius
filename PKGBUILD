# Maintainer: Evgeniy Alekseev <arcanis.arch at gmail dot com>
# Contributor: Matt Harrison <matt at mistbyte dot com>
# Contributor: Muhammad Qadri <Muhammad dot A dot Qadri at gmail dot com>

pkgname=tomxey-julius
pkgver=4.4.2.1
pkgrel=3
pkgdesc="A high-performance, two-pass large vocabulary continuous speech recognition decoder software"
arch=("x86_64")
url="http://julius.sourceforge.jp/"
license=("custom")
depends=("libpulse" "sdl2")
optdepends=("voxforge-am-julius: acoustic models")
provides=("julius")
conflicts=("julius")
source=("configure.patch"
        "tomxey-julius-repo.tar.gz")
md5sums=("SKIP" "SKIP")

prepare() {
  cd "${srcdir}"

  # configure error
  patch -p1 -i "${srcdir}/configure.patch"
}

build() {
  # redefine makeflags, coz it could not build in parallel
  export MAKEFLAGS="-j1"
  unset CFLAGS

  cd "${srcdir}/"
  ./configure \
    --prefix="/usr/" \
    --mandir='${prefix}/share/man'\
    --enable-words-int
  make
}

package() {
  cd "${srcdir}/"

  # leave static libraries since julius does not support
  # shared ones at the moment. See also FS#50599
  make prefix="${pkgdir}/usr" install
  install -Dm644 "LICENSE.txt" "${pkgdir}/usr/share/licenses/julius/LICENSE"
}

# vim:set ts=2 sw=2 et:
