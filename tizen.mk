#! /usr/bin/make -f

default:all

package?=iotivityexample

iotivity_logo_url?=http://research.cisco.com/img/devnet-page/tiles/iotivity.jpg

all?= tmp/512x512.png \
 shared/res/${package}.png

all: ${all}

icon.png: docs/logo.png Makefile
	convert -resize 117x117! $< $@

docs/screenshot.png: docs/logo.png
	echo cp $< $@

tmp/screenshot.jpg: docs/screenshot.png
	convert $< $@

tmp/117x177/%: %
	mkdir -p ${@D}
	convert -resize '117x117!' $< $@


tmp/512x512.png: docs/logo.png
	mkdir -p ${@D}
	convert -resize '512x512!' $< $@

tmp/512x512.jpg: docs/logo.png
	mkdir -p ${@D}
	convert -resize '512x512!' $< $@

distclean: clean
	rm -f *.wgt
	rm -rf Debug Release
	rm -rf .package-stamp .settings .sign
	rm -rf lib usr tmp

clean:
	rm -f *~

docs/logo.jpg:
	mkdir -p $@{D}
	wget -O $@.tmp ${iotivity_logo_url}
	convert $@.tmp $@


docs/logo.png: docs/logo.svg
	convert $< $@

shared/res/${package}.png: tmp/117x177/docs/logo.png
	cp $< $@


-include ${HOME}/bin/${package}.mk
