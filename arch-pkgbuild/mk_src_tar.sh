#!/usr/bin/bash
shopt -s extglob
rm tomxey-julius-repo.tar.gz
tar caf tomxey-julius-repo.tar.gz ../!(arch-pkgbuild)
