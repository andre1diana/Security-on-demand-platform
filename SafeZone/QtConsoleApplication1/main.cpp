#define _CRT_SECURE_NO_WARNINGS

#include<openssl/applink.c>
#include<iostream>

#include"Server.h"
#include"App.h"
#include"Codes.h"
#include"CriptoOperations.h"
#include"Logger.h"

int main(int argc, char* argv[])
{
    Server* server = &Server::getInstance();
    App* app = &App::getInstance();
    server->startServer();
    server->createConnection();



    int result;
    while (!server->isClosed())
    {
        int operationCode = server->receiveInt();
        switch (operationCode)
        {
        case LOGIN_VERIFY:
            app->getMenu().loginOption();
            break;

        case SIGNUP_SIG:
            app->getMenu().signupOption();
            break;

        case AES_CBC_ENC:
            app->getMenu().encryption(AES_CBC_ENC);
            break;

        case AES_OFB_ENC:
            app->getMenu().encryption(AES_OFB_ENC);
            break;

        case _3DES_ENC:
            app->getMenu().encryption(_3DES_ENC);
            break;

        case DECRYPT:
            app->getMenu().decryption();
            break;

        case SHA1_OPT:
            app->getMenu().hash(SHA1_OPT);
            break;

        case SHA2_OPT:
            app->getMenu().hash(SHA2_OPT);
            break;

        case SHA3_OPT:
            app->getMenu().hash(SHA3_OPT);
            break;

        case MAC_OPT:
            app->getMenu().mac();
            break;

        case RSA_OPT:
            app->getMenu().generateKeyOption(RSA_OPT);
            break;

        case DSA_OPT:
            app->getMenu().generateKeyOption(DSA_OPT);
            break;

        case DOWNLOAD_KEY:
            app->getMenu().downloadKey();
            break;

        case OPERATION_SUCCESS:
            std::cout << "Operation succes!\n";
            break;

        case OPERATION_FAILED:
            std::cout << "Operation failed!\n";
            break;

        case CERTIFICATE:
            app->getMenu().generateCertificateOption();
            break;

        case CLOSE_SIG:
            server->closeServer();
            App::destroyInstance();
            return 0;

        default:
            std::cout << "Invalid menu option!" << std::endl;
            return -1;
            //break;
        }


    }
    Logger::destroyInstance();
    return 0;
}