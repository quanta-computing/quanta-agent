#!/bin/sh

if [ -z "$1" ]; then
  echo "Usage: create_module.sh <name>"
  exit 1
fi

name=$1
mod_dir=$(dirname $0)

if [ -d $mod_dir/$name ]; then
  echo "Module $name already exists"
  exit 1
fi
cp -vr $mod_dir/template $mod_dir/$name
mv -v $mod_dir/$name/module.c $mod_dir/$name/$name.c
mv -v $mod_dir/$name/module.h $mod_dir/$name/$name.h
sed -i "" -e s/__NAME__/$name/g $mod_dir/$name/$name.c $mod_dir/$name/$name.h $mod_dir/$name/Makefile.am
