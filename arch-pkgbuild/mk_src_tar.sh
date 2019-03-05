#!/usr/bin/bash
shopt -s extglob
rm python-julius-repo.tar.gz
tar caf python-julius-repo.tar.gz ../!(arch-pkgbuild)
