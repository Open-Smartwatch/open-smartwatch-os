#pragma once

#ifdef KO_KR_PATCH  // This is a patch work to solve a special font problem.

// General terms
#define LANG_CONNECT "연결"
#define LANG_CONNECTED "연결됨"
#define LANG_DISCONNECT "연결해제"
#define LANG_MANUALLY "메뉴얼"
#define LANG_CANCEL "취소"
#define LANG_SAVE "저장"

// App: Time from web
#define LANG_TFW_UPDATE "동기화"

// App: BLE media control
#define LANG_BMC_VOLUME "음량"
#define LANG_BMC_CONNECTING "연결중..."

// App: Webserver
#define LANG_WEBSRV_STATION_PWD "디바이스암호"
#define LANG_WEBSRV_TITLE "설정"
#define LANG_WEBSRV_USER "사용자"
#define LANG_WEBSRV_PASS "비밀번호"

// App: Watchface Binary
#define LANG_WATCHFACE_BINARY_STEPS "걸음"

// Weekdays
#define LANG_MONDAY "월요일"
#define LANG_TUESDAY "화요일"
#define LANG_WEDNESDAY "수요일"
#define LANG_THURSDAY "목요일"
#define LANG_FRIDAY "금요일"
#define LANG_SATURDAY "토요일"
#define LANG_SUNDAY "일요일"

#endif

// At the very last: Include English as a fallback -> any keys not found in the current language, they will be defined in English
#include "en-US.h"
