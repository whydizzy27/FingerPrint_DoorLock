# FingerPrint_DoorLock
아트메가, 아두이노 키트 사용 지문인식 도어락 구현


### # 프로젝트 기간 및 참여 인원
2018.09 ~ 2018.12 / 1명


### # 개발 내용
- 지문 인식 모듈에 입력한 데이터를 기반으로 도어락 개폐 유무를 결정하여 서보 모터를 작동.
- CLCD, Dot Matrix, Buzzer를 활용하여 도어락 개폐 유무 상황을 더욱 가시화함.


### # 사용 언어 및 라이브러리
- Adafruit FingerPrint 라이브러리 http://www.adafruit.com/products/751


### # 개발 환경
- OS : Window
- Tool : Atmel Studio7, Arduino


### # HardWare
- Atmega128 탑재 JKit-128-1
- Arduino UNO
- 지문인식 모듈
- 서보모터
- CLCD
- 16x16 Dot Matrix
- Buzzer

### # Software
- atmega_code : atmel studio 에서 실행
- delete enroll : 지문 데이터 등록/삭제
- myfingerprint2 : 지문 데이터 일치여부 확인

