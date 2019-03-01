#include <cstring>
#include "horizon/core/applet.hpp"
#include "horizon/core/input.hpp"

namespace horizon {
    Applet::Applet(bool enableNxlinkStdio) :
         m_running(true),
         m_currentFrame(0) {
             consoleInit(NULL);
             socketInitializeDefault();
             if (enableNxlinkStdio) nxlinkStdio();
             appletInitializeGamePlayRecording();
         }

    Applet::~Applet() {
        appletEndBlockingHomeButton();
        socketExit();
        consoleExit(NULL);
    }

    bool Applet::isRunning() {
        if (!appletMainLoop()) return false;

        Input::scanInput();
        consoleUpdate(NULL);
        m_currentFrame++;

        return m_running;
    }

    void Applet::exit() {
        m_running = false;
    }

    int Applet::getCurrentFrame() {
        m_currentFrame = m_currentFrame % 60;
        return m_currentFrame;
    }

    // lib-applets
    void Applet::WiFiSelect::show() {
        AppletHolder applet;

        appletCreateLibraryApplet(&applet, AppletId_netConnect, LibAppletMode_AllForeground);

        appletHolderStart(&applet);
        while (appletHolderWaitInteractiveOut(&applet));
        appletHolderJoin(&applet);
        appletHolderClose(&applet);
    }

    Applet::PlayerSelect::User Applet::PlayerSelect::show() {
        struct UserReturnData {
            u64 result;
            u128 UID;
        } PACKED;

        struct UserReturnData outdata;

        AppletHolder aph;
        AppletStorage outputStorage;
        AppletStorage inputStorage, commonArgs;
        LibAppletArgs args;

        u8 indata[0xA0] = { 0 };
        indata[0x0] = 0;
        indata[0x96] = 1;

        appletCreateLibraryApplet(&aph, AppletId_playerSelect, LibAppletMode_AllForeground);
        libappletArgsCreate(&args, 0);
        libappletArgsPush(&args, &aph);

        appletCreateStorage(&inputStorage, 0xA0);
        writeCommonArguments(commonArgs, 0);

        appletStorageWrite(&inputStorage, 0, indata, 0xA0);
        appletHolderPushInData(&aph, &commonArgs);
        appletHolderPushInData(&aph, &inputStorage);
        appletHolderStart(&aph);

        while (appletHolderWaitInteractiveOut(&aph));

        appletHolderJoin(&aph);
        appletHolderPopOutData(&aph, &outputStorage);
        appletStorageRead(&outputStorage, 0, &outdata, 24);

        appletHolderClose(&aph);
        appletStorageClose(&outputStorage);

        AccountProfile profile;
        AccountProfileBase base;
        AccountUserData data;

        accountGetProfile(&profile, outdata.UID);
        accountProfileGet(&profile, &data, &base);

        return {
            base.username,
            (long long int) base.userID,
            data.iconID,
            data.iconBackgroundColorID
        };
    }

    Applet::Error::Error() :
        m_title(""),
        m_description(""),
        m_moduleCode(2000),
        m_descriptionCode(0) { /* do nothing */ }

    void Applet::Error::show(const std::string& title, const std::string& description, int descriptionCode, int moduleCode) {
        AppletHolder err;
        AppletStorage errStor, commonArgs;
        LibAppletArgs errArgs;

        appletCreateLibraryApplet(&err, AppletId_error, LibAppletMode_AllForeground);
        libappletArgsCreate(&errArgs, 1);
        libappletArgsPush(&errArgs, &err);

        appletCreateStorage(&errStor, 4120);
        writeCommonArguments(commonArgs);

        u8 args[4120] = {0};
        args[0] = 1;

        *(u64*)&args[8] = moduleCode;
        *(u64*)&args[12] = descriptionCode;
        strcpy((char*) &args[24], title.c_str());
        strcpy((char*) &args[2072], description.c_str());
        appletStorageWrite(&errStor, 0, args, 4120);

        appletHolderPushInData(&err, &errStor);
        appletHolderPushInData(&err, &commonArgs);

        appletHolderStart(&err);
        appletHolderJoin(&err);
        appletHolderClose(&err);
    }

    void Applet::Error::show() {
        show(m_title, m_description, m_descriptionCode, m_moduleCode);
    }

    void Applet::Error::setTitle(const std::string& title) {
        m_title = title;
    }

    const std::string& Applet::Error::getTitle() {
        return m_title;
    }

    void Applet::Error::setDescription(const std::string& description) {
        m_description = description;
    }

    const std::string& Applet::Error::getDescription() {
        return m_description;
    }

    void Applet::Error::setModuleCode(int code) {
        m_moduleCode = code;
    }

    int Applet::Error::getModuleCode() {
        return m_moduleCode;
    }

    void Applet::Error::setDescriptionCode(int code) {
        m_descriptionCode = code;
    }

    int Applet::Error::getDescriptionCode() {
        return m_descriptionCode;
    }

    void Applet::Error::setErrorCode(int module, int description) {
        m_moduleCode = module;
        m_descriptionCode = description;
    }

    // private
    void Applet::writeCommonArguments(AppletStorage& t_storage, int t_version) {
        appletCreateStorage(&t_storage, 0x20);
        u8 args[0x20] = {0};

        args[0x0] = t_version; // version
        args[0x4] = 0x20;      // size
        args[0xC] = 0;         // theme color
        args[0x10] = 0;        // play startup sound
        args[0x18] = svcGetSystemTick(); // system tick

        appletStorageWrite(&t_storage, 0, args, 0x20);
    }
} /* horizon */
