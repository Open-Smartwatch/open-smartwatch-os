#ifndef LANG_KO_H
#define LANG_KO_H

// General terms
#define LANG_CONNECT "연결"
#define LANG_CONNECTED "연결됨"
#define LANG_DISCONNECT "연결해제"

// App: Time from web
#define LANG_TFW_UPDATE "동기화"
#define LANG_MANUALLY "수동설정"
#define LANG_CANCEL "취소"
#define LANG_SAVE "저장"

// App: BLE media control
#define LANG_BMC_VOLUME "음량"
#define LANG_BMC_CONNECTING "연결중..."

#ifndef KO_KR_PATCH  // This is a patch work to solve a special font problem.

// App: Webserver
#define LANG_WEBSRV_STATION_PWD "Station Password:"
#define LANG_WEBSRV_TITLE "Configuration"
#define LANG_WEBSRV_USER "User:"
#define LANG_WEBSRV_PASS "Password:"

// App: Watchface Binary
#define LANG_WATCHFACE_BINARY_STEPS "steps"
#elif

// App: Webserver
#define LANG_WEBSRV_STATION_PWD "디바이스암호"
#define LANG_WEBSRV_TITLE "설정"
#define LANG_WEBSRV_USER "사용자"
#define LANG_WEBSRV_PASS "비밀번호"

// App: Watchface Binary
#define LANG_WATCHFACE_BINARY_STEPS "걸음"
#endif

// Weekdays
#define LANG_MONDAY "월요일"
#define LANG_TUESDAY "화요일"
#define LANG_WEDNESDAY "수요일"
#define LANG_THURSDAY "목요일"
#define LANG_FRIDAY "금요일"
#define LANG_SATURDAY "토요일"
#define LANG_SUNDAY "일요일"

#endif
