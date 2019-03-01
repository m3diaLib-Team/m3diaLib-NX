/**
 * @file applet.hpp
 * @brief Defines the applet class (used for running the app)
 */
#pragma once

#ifndef APPLET_H
#define APPLET_H

#include <string>
#include "horizon/libnx.hpp"

/**
 * @brief The general horizon-namespace
 */
namespace horizon {
    /**
     * @brief The applet class (used for running the app)
     */
    class Applet {
    public:
        /**
         * @brief Defines the different operation modes of the console
         */
        enum class OperationMode {
            Handheld, ///< Handheld
            Docked    ///< Docked
        };

        /**
         * @brief Creates the applet
         * @param enableNxlinkStdio Whether you want to enable nxlinkStdio or not
         */
        Applet(bool enableNxlinkStdio = true);

        /**
         * @brief Destructs the applet
         */
        ~Applet();

        /**
         * @brief Returns if the applet is still running
         */
        bool isRunning();

        /**
         * @brief Exits the applet
         */
        void exit();

        /**
         * @brief Sets whether or not to allow the use of the home button
         * @param allow `true` if the user is allowed to use the home button, `false` otherwise
         */
        static inline void allowHomeButton(bool allow) {
            if (allow) {
                appletEndBlockingHomeButton();
            } else {
                appletBeginBlockingHomeButton(0);
            }
        }

        /**
         * @brief Enables/disables video recording
         * @param enable Whether or not to enable video recording
         */
        static inline void enableVideoRecording(bool enable) {
            if (enable) {
                appletSetGamePlayRecordingState(1);
            } else {
                appletSetGamePlayRecordingState(0);
            }
        }

        /**
         * @brief Returns the current operation mode of the console
         * @return The current operation mode
         */
        static inline OperationMode getOperationMode() {
            return (OperationMode) appletGetOperationMode();
        }

        /**
         * @brief Returns the current frame
         * @return The currently drawn frame (ranging from 0 to 59)
         */
        int getCurrentFrame();

        /** @addtogroup Library-Applets
         *  @{
         */
        /**
         * @brief The WiFi-select screen
         */
        class WiFiSelect {
        public:
            /**
             * @brief Displays the library-applet
             */
            static void show();
        };

        /**
         * @brief The player-select screen
         */
        class PlayerSelect {
        public:
            /**
             * @brief Represents a user account
             */
            struct User {
                const std::string username; ///< Username
                long long int userId;       ///< User ID
                unsigned int iconId;        ///< Icon ID
                short int backgroundColor;  ///< Background color
            };

            /**
             * @brief Displays the library-applet
             */
            static User show();
        };

        /**
         * @brief The error-screen applet
         * @warning This creates an entry in the error-log in the system settings that contains the error-code, the title and the description as well as the app.
         */
        class Error {
        public:
            /**
             * @brief Default constructor
             */
            Error();

            /**
             * @brief Quickly displays an error with the given title and description
             * @param title       The title
             * @param description The description (shown when the user clicks on "Details")
             */
            static void show(const std::string& title, const std::string& description, int minorCode = 0, int majorCode = 2000);

            /**
             * @brief Displays the error
             */
            void show();

            /**
             * @brief Sets the title of the error
             * @param title The title
             */
            void setTitle(const std::string& title);

            /**
             * @brief Returns the title of the error
             * @return The title
             */
            const std::string& getTitle();

            /**
             * @brief Sets the description of the error
             * @param description The description (shown when the user clicks on "Details")
             */
            void setDescription(const std::string& description);

            /**
             * @brief Returns the description of the error
             * @return The description (shown when the user clicks on "Details")
             */
            const std::string& getDescription();

            /**
             * @brief Sets the major code of the error
             * @param code The major code (the first part of the error code)
             */
            void setMajorCode(int code);

            /**
             * @brief Returns the major code of the error
             * @return The major code (the first part of the error code)
             */
            int getMajorCode();

            /**
             * @brief Sets the major code of the error
             * @param code The major code (the first part of the error code)
             */
            void setMinorCode(int code);

            /**
             * @brief Returns the minor code of the error
             * @return The minor code (the second part of the error code)
             */
            int getMinorCode();

            /**
             * @brief Sets the code of the error
             * @param minorCode The minor code (the second part of the error code)
             * @param majorCode The major code (the first part of the error code)
             */
            void setErrorCode(int minorCode, int majorCode = 2000);
        private:
            /* data */
            std::string m_title, m_description;
            int m_majorCode, m_minorCode;
        };
        /** @}*/

    private:
        static void writeCommonArguments(AppletStorage& t_storage, int t_version = 1);

        /* data */
        bool m_running;
        int m_currentFrame;
    };
} /* horizon */

#endif /* end of include guard: APPLET_H */
