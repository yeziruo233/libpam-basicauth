#include <string.h>
#include <syslog.h>
#include <curl/curl.h>
#include <security/pam_appl.h>
#include <security/pam_ext.h>
#include <security/pam_modules.h>


int pam_sm_authenticate(pam_handle_t *p, int flags, int argc, const char **argv) {
    const char *username;
    const char *password;
    const char *authentication_url = NULL;
    const char *user_agent = NULL;

    for (int i = 0; i < argc; i++) {
        if (strncmp(argv[i], "url=", strlen("url=")) == 0) {
            authentication_url = argv[i] + strlen("url=");
        } else if (strncmp(argv[i], "user_agent=", strlen("user_agent=")) == 0) {
            user_agent = argv[i] + strlen("user_agent=");
        }
    }

    if (authentication_url == NULL) {
        pam_syslog(p, LOG_ERR, "authentication url is null");
        return PAM_AUTHINFO_UNAVAIL;
    }

    if (pam_get_user(p, &username, NULL) != PAM_SUCCESS) {
        pam_syslog(p, LOG_ERR, "couldn't get username");
        return PAM_AUTH_ERR;
    }

    if (pam_get_authtok(p, PAM_AUTHTOK, &password, NULL) != PAM_SUCCESS) {
        pam_syslog(p, LOG_ERR, "couldn't get password");
        return PAM_AUTH_ERR;
    }

    CURL *curl;
    curl = curl_easy_init();
    int result = PAM_AUTH_ERR;
    if (curl) {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, authentication_url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        curl_easy_setopt(curl, CURLOPT_USERNAME, username);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        if (user_agent != NULL) {
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
        }
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            pam_syslog(p, LOG_ERR, "%s", curl_easy_strerror(res));
        } else {
            int code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
            if (code == 200) {
                result = PAM_SUCCESS;
            }
        }
    } else {
        pam_syslog(p, LOG_ERR, "unable to initialize libcurl");
    }
    curl_easy_cleanup(curl);
    return result;
}

int pam_sm_setcred(pam_handle_t *p, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}
