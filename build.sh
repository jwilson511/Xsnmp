BASEDIR=$PWD

sudo rm -rf /Library/Xsnmp

cd deps
tar zxvf pcre-8.02.tar.gz
cd pcre-8.02
$BASEDIR/util/pcre_build.sh
cd $BASEDIR

/Xcode3/usr/bin/make distclean
PATH="/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH" \
./bootstrap_osx.sh && /Xcode3/usr/bin/make && sudo /Xcode3/usr/bin/make install

sudo cp launchd/com.xsnmp.xsnmp-agentx.plist /Library/Xsnmp/XsnmpAgentExtension.app/Resources
sudo cp -r packaging/XsnmpInstaller.pmdoc /Library/Xsnmp/XsnmpAgentExtension.app/Resources
sudo cp -r packaging/Finished.rtfd /Library/Xsnmp/XsnmpAgentExtension.app/Resources
sudo cp -r packaging/scripts /Library/Xsnmp/XsnmpAgentExtension.app/Resources
sudo cp /Library/Xsnmp/XsnmpAgentExtension.app/Resources/scripts/postinstall.template /Library/Xsnmp/XsnmpAgentExtension.app/Resources/scripts/postinstall
sudo ./packaging/scripts/prepare_postinstall.sh
sudo cp packaging/Info.plist /Library/Xsnmp/XsnmpAgentExtension.app/Contents 
sudo cp util/xsnmp_debug_info.sh /Library/Xsnmp/XsnmpAgentExtension.app/Contents/MacOS
chnod ug+x /Library/Xsnmp/XsnmpAgentExtension.app/Contents/MacOS/xsnmp_debug_info.sh

cd prefpane
MVERS=`agvtool mvers | grep 'Found CFBundleShortVersionString' | sed 's/^[^\"]*\"//g' | sed 's/".*//g'`
VERS=`agvtool vers | egrep '^[ ]*[0-9]+' | sed 's/^[ ]*//g'`
PATH="/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH" \
/Xcode3/usr/bin/xcodebuild -configuration Release clean
PATH="/Xcode3/usr/bin:/Xcode3/usr/sbin:$PATH" \
/Xcode3/usr/bin/xcodebuild -configuration Release build
cd ..

sudo cp -r prefpane/build/release/Xsnmp.prefPane /Library/Xsnmp/XsnmpAgentExtension.app/Resources

/Xcode3/usr/bin/packagemaker --doc "/Library/Xsnmp/XsnmpAgentExtension.app/Resources/XsnmpInstaller.pmdoc" --out "packaging/Xsnmp-Installer-$MVERS.pkg" --title "Xsnmp $MVERS Installer"

#sudo rm -rf /Library/Xsnmp
