#!/bin/sh

cd `dirname $0`/paint
../build/xviweb/src/xviweb/xviweb --loadResponder ../build/xviweb/src/FileResponder/libFileResponder.so --loadResponder ../build/src/libxvipaint.so --defaultRoot .
