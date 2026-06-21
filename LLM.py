import serial
import json
import ollama
import re
import csv
import os
import time
from datetime import datetime

# 데이터 저장 경로 설정
LOG_DIR = "C:/Environment" if os.path.exists("C:/Environment") else "D:/Environment"
if not os.path.exists(LOG_DIR): os.makedirs(LOG_DIR)
LOG_FILE = os.path.join(LOG_DIR, f"farm_log_{datetime.now().strftime('%Y%m%d')}.csv")

# 보레이트 115200으로 최적화 및 타임아웃 0.05초로 단축
ser = serial.Serial('COM14', 115200, timeout=0.05)

# 상태 저장 변수
last_save_time = 0
SAVE_INTERVAL = 60
last_temp_cmd = 'z'
last_vwc_cmd = 'z'


def save_to_csv(data, temp_cmd, vwc_cmd, force_save=False):
    global last_save_time
    current_time = time.time()
    if force_save or (current_time - last_save_time >= SAVE_INTERVAL):
        try:
            file_exists = os.path.isfile(LOG_FILE)
            with open(LOG_FILE, mode='a', newline='', encoding='utf-8') as f:
                writer = csv.writer(f)
                if not file_exists:
                    header = ['timestamp'] + list(data.keys()) + ['temp_cmd', 'vwc_cmd']
                    writer.writerow(header)
                row = [datetime.now().strftime('%Y-%m-%d %H:%M:%S')] + list(data.values()) + [temp_cmd, vwc_cmd]
                writer.writerow(row)
            last_save_time = current_time
            print(f">> [저장 성공] {LOG_FILE}")
            return True
        except Exception as e:
            print(f">> [저장 실패] 원인: {e}")
            return False
    return False


def ask_ollama(data_json):
    prompt = f"""
    너는 스마트팜 상추 재배 전문가야.
    [환경 데이터]: {data_json}
    [절대 규칙]: 온도 < 15.0 또는 > 30.0, 수분 < 10.0 또는 > 30.0 은 즉시 제어.
    [판단 기준]: 상추 생육 최적 온도(20-25도), 최적 수분(15-25%) 유지.
    [출력]: 명령어만 출력 (a, b, c, d, z)
    """
    try:
        response = ollama.chat(model='qwen2:1.5b', messages=[{'role': 'user', 'content': prompt}])
        match = re.search(r'[a-dA-Dz]', response['message']['content'])
        return match.group(0).lower() if match else 'z'
    except:
        return 'z'


print("스마트팜 제어 시스템 시작...")

# [워밍업 코드] 시스템 시작 시 모델을 미리 로딩하여 상주 상태로 전환
print("AI 모델을 워밍업 중입니다...")
ask_ollama({"status": "warm_up"})
print("AI 모델 준비 완료. 실시간 제어를 시작합니다.")

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        start_idx, end_idx = line.find('{'), line.rfind('}')

        if start_idx != -1 and end_idx != -1:
            try:
                data = json.loads(line[start_idx:end_idx + 1])
                print(f"데이터 수신: {data}")
                temp, vwc = data.get('air_temperature', 0), data.get('vwc', 0)

                cmd_temp, cmd_vwc = 'z', 'z'

                # 1. 안전 제어 (절대 규칙 - 최우선 즉시 전송)
                cmd_temp = 'a' if temp >= 35.0 else ('b' if temp <= 30.0 else 'z')
                cmd_vwc = 'c' if vwc <= 10.0 else ('d' if vwc >= 20.0 else 'z')

                if cmd_temp != 'z': ser.write(cmd_temp.encode()); ser.flush()
                if cmd_vwc != 'z': ser.write(cmd_vwc.encode()); ser.flush()

                # 2. AI 지능 제어 (안전 구간일 때만)
                if cmd_temp == 'z' and cmd_vwc == 'z':
                    ai_cmd = ask_ollama(data)
                    if ai_cmd in ['a', 'b']:
                        cmd_temp = ai_cmd
                        ser.write(cmd_temp.encode());
                        ser.flush()
                    elif ai_cmd in ['c', 'd']:
                        cmd_vwc = ai_cmd
                        ser.write(cmd_vwc.encode());
                        ser.flush()

                # 3. 상태 출력 및 로깅
                changed_temp = (cmd_temp != last_temp_cmd)
                changed_vwc = (cmd_vwc != last_vwc_cmd)

                print(f">> [상태] Temp: {temp}°C({cmd_temp.upper()}), VWC: {vwc}%({cmd_vwc.upper()})")
                save_to_csv(data, cmd_temp, cmd_vwc, force_save=(changed_temp or changed_vwc))

                last_temp_cmd, last_vwc_cmd = cmd_temp, cmd_vwc

            except Exception as e:
                print(f"처리 오류: {e}")
                continue
