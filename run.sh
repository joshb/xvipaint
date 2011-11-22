#!/bin/sh

cd `dirname $0`/paint
../build/xviweb/src/xviweb/xviweb --address 0.0.0.0 --loadResponder ../build/xviweb/src/FileResponder/libFileResponder.so --loadResponder ../build/src/libxvipaint.so --defaultRoot .
