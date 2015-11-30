.PHONY: translate qmlformelements-github2farcaster qmlformelements-farcaster2github test

translate:
	/opt/qt/bin/lupdate -verbose -no-obsolete ./kullodesktop -ts ./translations/farcaster_de.ts

qmlformelements-github2farcaster:
	cp ~/nobackup/QmlFormElements/F* ./kullodesktop/ui/formelements/

qmlformelements-farcaster2github:
	cp ./kullodesktop/ui/formelements/F* ~/nobackup/QmlFormElements/

test:
	./check_qmllint.sh
	./check_qinvokable_namespaces.sh
