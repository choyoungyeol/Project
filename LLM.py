import serial
import json
import ollama
import re

ser = serial.Serial('COM14', 9600, timeout=1)


def ask_ollama(data_json):
    prompt = f"""
    당신은 스마트팜 제어 AI입니다. 현재 상태: {data_json}

    [제어 규칙]
    1. 온도(temp)가 35도 이상이면 'a'(Relay1 ON), 30도 이하이면 'b'(Relay1 OFF).
    2. 수분함량(vwc)이 30% 이하이면 'c'(Relay2 ON), 40% 이상이면 'd'(Relay2 OFF).
    3. 조건에 해당하지 않으면 아무것도 하지 마세요.
    """
    try:
        response = ollama.chat(model='llama3:latest', messages=[{'role': 'user', 'content': prompt}])
        match = re.search(r'[a-dA-Dz]', response['message']['content'])
        return match.group(0).lower() if match else 'z'
    except:
        return 'z'


print("스마트팜 제어 시스템 시작 (지능형 하이브리드 모드)...")

while True:
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line or not line.startswith('{'): continue

        try:
            data = json.loads(line)
            # 모든 데이터 출력
            print("-" * 30)
            for k, v in data.items(): print(f"{k}: {v}")

            temp = data.get('air_temperature', 0)
            vwc = data.get('vwc', 0)

            # [단계 1] 하드코딩된 안전 제어 (최우선순위)
            acted_by_safety = False
            if temp >= 35.0:
                ser.write(b'a'); print("[안전 제어] 온도 35도 이상: Relay1 ON (a)"); acted_by_safety = True
            elif temp <= 30.0:
                ser.write(b'b'); print("[안전 제어] 온도 30도 이하: Relay1 OFF (b)"); acted_by_safety = True

            if vwc <= 10.0:
                ser.write(b'c'); print("[안전 제어] 수분 10% 이하: Relay2 ON (c)"); acted_by_safety = True
            elif vwc >= 30.0:
                ser.write(b'd'); print("[안전 제어] 수분 30% 이상: Relay2 OFF (d)"); acted_by_safety = True

            # [단계 2] 안전 범위일 경우에만 AI 제어 수행
            if not acted_by_safety:
                cmd = ask_ollama(data)
                if cmd != 'z':
                    ser.write(cmd.encode())
                    print(f"[AI 지능 제어] 판단 결과: {cmd}")
                else:
                    print("[상태] 정상 범위: 추가 조치 불필요")

        except json.JSONDecodeError:
            continue
