.PHONY: translate qmlformelements-github2farcaster qmlformelements-farcaster2github test

translate:
	lupdate -verbose -no-obsolete ./kullodesktop -ts ./translations/farcaster_de.ts

qmlformelements-github2farcaster:
	cp ~/nobackup/QmlFormElements/F* ./kullodesktop/ui/formelements/

qmlformelements-farcaster2github:
	cp ./kullodesktop/ui/formelements/F* ~/nobackup/QmlFormElements/
