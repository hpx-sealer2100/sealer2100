class App:
    account = "계정"
    scan = "거래 스캔"
    nft = "NFT 전시실"
    guide = "사용 가이드"
    security = "보안"
    setting = "설정"

#### Setting App
class Setting:
    bluetooth = "블루투스"
    language = "언어"
    vibration = "터치 피드백"
    brightness = "화면 밝기"
    auto_lock = "자동 잠금"
    auto_shutdown = "자동 종료"
    animation = "전환 애니메이션"
    wallpaper = "배경화면"
    power_off = "전원 끄기"
    restart = "재시작"

#### Security App
class Security:
    change_pin = "PIN 코드 변경"
    backup_mnemonic = "니모닉 백업"
    check_mnemonic = "니모닉 확인"
    wipe_device = "기기 초기화"

#### guide App
class Guide:
    about = "Digital Shield 정보"
    terms_of_use = '이용약관'
    device_info = '기기 정보'
    firmware_update = '펌웨어 업데이트'
    terms_title_terms_us = 'Digit Shield 이용약관'
    terms_describe_terms_us = '이용약관 전문을 보시려면 아래 링크를 방문해 주세요:\n http://hypermate.com/terms'

    terms_title_product_services = 'Digit Shield 제품 및 서비스'
    terms_describe_product_services = '당사의 하드웨어 지갑은 암호화폐를 안전하게 관리합니다'
    terms_title_risks = '위험 안내'
    terms_describe_risks = '암호화폐 및 기술적 취약성과 관련된 위험에 유의하십시오.'
    terms_title_disclaimers = '면책조항'
    terms_describe_disclaimers = '제공되는 정보는 재무적 조언이 아닙니다. 결정을 내리기 전에 전문가의 조언을 구하십시오.'
    terms_title_contact_us = '문의하기'
    terms_describe_contact_us = '궁금한 사항이 있으면 www.ds.pro@gmail.com으로 이메일을 보내주세요.'

    accept_tems = '1. 약관 동의'
    use_range = '1. 적용 범위'
    range_include = '이 약관은 Digital Shield 지갑을 통해 제공되는 모든 서비스에 적용됩니다.'
    range_include_1 = '하드웨어 지갑의 구매, 활성화, 사후 서비스; Digital Shield 모바일 앱(안드로이드/iOS/Goolg)의 다운로드, 설치, 기능 사용; 펌웨어 업그레이드 서비스(보안 패치, 기능 향상 버전 포함); 다중 체인 디지털 자산 관리(BTC, ETH 등 3000개 이상의 토큰 저장 및 전송 지원); 기술 지원(장치 고장 해결, 트랜잭션 서명 이상 처리 등).'
    user_qualification = '2. 사용자 자격'
    ability_include = '귀하는 18세 이상이며 완전한 민사 행위 능력을 가지고 있음을 확인합니다.'
    ability_include_1 = '귀하가 거주하는 법적 관할 구역에서는 암호화폐 및 관련 하드웨어 장치의 사용이 금지되어 있지 않습니다(예: 중국 본토 사용자는 사용 리스크를 스스로 부담해야 합니다).'
    terms_update_infor = '3. 약관 업데이트 및 통지'
    update_infor_content = '회사는 일방적으로 약관을 수정할 권리를 보유합니다. 개정 내용은 공식 웹사이트의 공지판에 게재되며, 효력 발생일은 공지에 따릅니다. 서비스를 계속 사용하는 경우 개정된 약관에 동의한 것으로 간주됩니다. 동의하지 않는 경우 효력 발생일 전에 서면으로 통지하고 사용을 중단해야 합니다.'

    wallet_buy_iterms = '2. 하드웨어 지갑 구매 조항'
    order_process = '1. 주문 프로세스'
    payment_confirmation = '주문 결제가 성공한 후(블록체인 네트워크 확인 또는 은행 입금을 기준으로), 시스템은 24시간 이내에 상태를 업데이트합니다.'
    inventory_shortage = '재고 부족: 재고가 부족한 경우, 사용자는 선택할 수 있습니다.'
    inventory_shortage_1 = 'a. 재고 보충을 기다립니다(최대 30일, 기간을 초과하면 자동으로 환불됩니다).'
    inventory_shortage_2 = 'b. 원래 방법으로 전액 환불(암호화폐 주문은 결제 시 환율로 계산됩니다).'
    return_and_exchange_policy = '2. 반품 및 교환 정책'
    return_and_exchange_condi = '반품 조건:'
    return_and_exchange_condi_con = 'a. 활성화되지 않은 장치는 원래 공장 시ール 스티커(주문 번호와 일치) 및 완전한 액세서리(USB 케이블, 설명서, 니모닉 카드)를 보관해야 합니다. b. 반품 신청은 수령 후 7일 이내에 제출해야 하며, 기간을 초과하면 품질 검사 합격으로 간주됩니다. c. 반품 배송비는 사용자가 부담합니다(품질 문제 제외).'
    warranty_scope = '보증 범위:'
    warranty_scope_1 = 'a. 안전 칩 고장, 디스플레이 이상, 버튼 기능 상실 등 인위적인 손상이 아닌 문제를 보장합니다.'
    warranty_scope_2 = 'b. 구매 증명(주문 번호) 및 고장 증명(동영상은 장치의 SN 번호 및 이상 현상을 명확하게 보여야 합니다)을 제공해야 합니다.'
    warranty_scope_3 = 'c. 인위적인 손상(물에 빠짐, 추락 등)은 보증 범위 밖이며, 유료 수리가 가능합니다.'

    disclaimer = '3. 면책 조항'
    product_risk = '1. 제품 리스크'
    physical_risk = '물리적 리스크:'
    physical_risk_1 = 'a. 장치는 고온(>60℃), 고습도(>90% RH), 강력한 자기장(>100mT)의 환경에서 기능하지 않을 수 있습니다.'
    physical_risk_2 = 'b. 장기간 충전되지 않은 경우 배터리가 손상될 수 있습니다(매월 1회 충전을 권장합니다).'
    supply_chain_risk = '공급망 리스크:'
    supply_chain_risk_1 = 'a. 공식 웹사이트는 위조 방지 검증 도구를 제공하며, 장치의 QR 코드를 스캔하여 진품 여부를 확인할 수 있습니다.'
    supply_chain_risk_2 = 'b. 장치가 교체된 것으로 의심되는 경우 즉시 고객 서비스에 연락하고 경찰에 신고해야 합니다.'
    service_interruption = '2. 서비스 중단'
    service_interruption_1 = '계획된 유지보수는 공식 웹사이트 공지를 통해 48시간 전에 통지됩니다. 긴급 유지보수의 경우 사전 통지 없이 서비스가 일시 중단될 수 있습니다.'
    service_interruption_2 = '불가항력적인 요인으로 인한 데이터 손실에 대해서는 배상 책임을 지지 않습니다.'


    device_label = "장비 이름"
    device_title_firmware_version = '펌웨어 버전'
    device_title_serial_number = '시리얼 번호'
    bluetooth_name = "블루투스 이름"
    bluetooth_version = "블루투스 버전"
    firmware_title_1 = '1. 장치의 배터리가 20% 이상인지 확인하세요'
    firmware_title_2 = '2. USB-C 케이블로 장치를 컴퓨터에 연결하세요'
    firmware_title_3 = '3. 「펌웨어 업그레이드」를 클릭하세요'
    firmware_title_caution = '경고'
    firmware_describe_caution = '업그레이드 중에는 USB 연결이 유지되도록 하세요'
    equipment_info = '장비 정보'
    equipment_name = '장비 이름'
    equipment_version = '장비 버전'

class Nft:
    nft_item ="{} 아이템"
    nft_items ="{} 아이템들"
class Title:
    enter_old_pin = "기존 PIN 코드를 입력하세요"
    enter_new_pin = "새 PIN 코드를 입력하세요"
    enter_pin = "PIN 코드를 입력하세요"
    enter_pin_again = "PIN 코드를 다시 입력하세요"
    select_language = "언어"
    create_wallet = "지갑 생성"
    wallet = "지갑"
    import_wallet = "지갑 가져오기"
    restore_wallet = "지갑 복구"
    wallet_is_ready = "지갑 준비 완료"
    select_word_count = "단어 개수 선택"
    wallet_security = "지갑 보안"
    pin_security = "PIN 보안 안내"
    mnemonic_security = "니모닉 보안 안내"
    backup_mnemonic = "니모닉 백업"
    enter_mnemonic = "니모닉을 입력하세요"
    check_mnemonic = "니모닉 확인"
    success = "성공"
    operate_success = "작업 성공"
    theme_success = "테마 변경 성공"
    warning = "경고"
    error = "오류"
    verified = "니모닉 백업 완료"
    invalid_mnemonic = "유효하지 않은 니모닉"
    pin_not_match = "PIN 불일치"
    check_recovery_mnemonic = "복구 니모닉 확인"
    mnemonic_not_match = "니모닉 불일치"
    power_off = "전원 끄기"
    restart = "재시작"
    change_language = "언어 변경"
    wipe_device = "기기 초기화"
    bluetooth_pairing = "블루투스 페어링"
    address="{} 주소"
    public_key = "{} 공개키"
    xpub = "{} XPub"
    transaction = "{} 트랜잭션"
    transaction_detail = "트랜잭션 상세정보"
    confirm_transaction = "트랜잭션 확인"
    confirm_message = "메시지 확인"
    signature = "서명 결과"
    wrong_pin = "PIN 오류"
    pin_changed = "PIN 변경됨"
    pin_enabled = "PIN 활성화됨"
    pin_disabled = "PIN 비활성화됨"
    unknown_token = "알 수 없는 토큰"
    view_data = "데이터 보기"
    sign_message = "{} 메시지 서명"
    verify_message = "{} 메시지 검증"
    typed_data = "{} 구조화 데이터"
    typed_hash = "{} 구조화 해시"
    system_update = "시스템 업데이트"
    entering_boardloader = "보드로더 진입 중"
    remove_credential = "인증서 삭제"
    list_credentials = "인증서 목록"
    authorize_coinjoin = "CoinJoin 승인"
    multisig_address_m_of_n = "{} 다중서명 주소\n({} of {})"
    u2f_register = "U2F 등록"
    u2f_unregister = "U2F 등록 해제"
    u2f_authenticate = "U2F 인증"
    fido2_register = "FIDO2 등록"
    fido2_unregister = "FIDO2 등록 해제"
    fido2_authenticate = "FIDO2 인증"
    finalize_transaction = "거래 완료"
    meld_transaction = "거래 통합"
    update_transaction = "거래 업데이트"
    high_fee = "높은 요금"
    fee_is_high = "수수료 과다"
    confirm_locktime = "잠금 시간 확인"
    view_transaction = "거래 확인"
    x_confirm_payment = "{} 결제 확인"
    confirm_replacement = "거래 대체 확인"
    x_transaction = "{} 거래"
    x_joint_transaction = "{} 공동 거래"
    change_label = "기기 이름 변경"
    enable_passphrase = "Passphrase 활성화"
    disable_passphrase = "Passphrase 비활성화"
    passphrase_source = "Passphrase 입력 설정"
    enable_safety_checks ="안전 검사 활성화"
    disable_safety_checks ="안전 검사 비활성화"
    experiment_mode = "실험 모드"
    set_as_homescreen = "홈화면으로 설정"
    get_next_u2f_counter = "U2F 카운터 가져오기"
    set_u2f_counter = "U2F 카운터 설정"
    encrypt_value = "데이터 암호화"
    decrypt_value = "데이터 복호화"
    confirm_entropy = "엔트로피 추출"
    memo = "메모"
    import_credential = "인증서 가져오기"
    export_credential = "인증서 내보내기"
    asset = "자산"
    unimplemented = "구현되지 않음"
    invalid_data="잘못된 데이터 형식"
    low_power = "배터리 부족"
    collect_nft = "NFT 수집"
    verify_device = "장치 검증"
    update_bootloader = "부트로더 업데이트"
    update_resource = "리소스 업데이트"
    need_input_tips = "모든 보안어를 입력한 후 다음 단계를 클릭하십시오."
    need_select_tips = "보안어 순서대로 클릭하여 선택한 후 완료되면 다음 단계를 클릭하십시오."
    return_check_mnemonic = "보안어 확인하기"

class Text:
    tap_to_unlock = "잠금 해제를 터치하세요"
    unlocking = "기기 잠금 해제 중..."
    str_words = "#FFFFFF {}# 단어"
    backup_manual = "니모닉을 수동으로 기록하여 안전한 장소에 보관하세요"
    check_manual = "아래 단어들을 순서대로 터치해주세요"
    backup_verified = "니모닉 백업이 완료되었습니다. 안전하게 보관하고 누구와도 공유하지 마세요"
    backup_invalid = "입력한 니모닉이 올바르지 않습니다. 백업한 니모닉을 확인 후 다시 시도해주세요"
    pin_not_match = "입력한 PIN 코드가 올바르지 않습니다. 다시 시도해주세요"
    please_wait = "잠시만 기다려주세요"
    wiping_device = "기기 데이터 삭제 중..."
    create_wallet = "새 니모닉 생성으로 지갑 만들기"
    restore_wallet = "백업한 니모닉으로 지갑 복구하기"
    restore_mnemonic_match = "니모닉이 일치합니다. 백업이 정확하게 이루어졌습니다"
    restore_success = "지갑 복구 성공"
    create_success = "니모닉 백업이 완료되었으며 지갑이 생성되었습니다"
    check_recovery_mnemonic = "니모닉을 확인하고 완전히 일치하는지 확인해주세요"
    invalid_recovery_mnemonic = "입력한 니모닉이 유효하지 않습니다. 니모닉을 확인 후 다시 시도해주세요"
    check_recovery_not_match = "입력한 니모닉은 유효하지만, 기기에 저장된 니모닉과 일치하지 않습니다"
    shutting_down = "전원을 끄는 중..."
    restarting = "재시작하는 중..."
    never = "사용 안 함"
    second = "초"
    seconds = "초"
    minute = "분"
    minutes = "분"
    changing_language = "언어를 변경합니다\n이 설정을 적용하면 기기가 재시작됩니다"
    change_pin = "4~16자리 PIN 코드를 설정하여 기기를 보호하세요"
    wipe_device = "기기를 공장 초기 상태로 복구합니다.\n경고: 기기의 모든 데이터가 삭제됩니다"
    wipe_device_check = [
        "기기 초기화 시 모든 데이터가 삭제됩니다",
        "삭제된 데이터는 복구할 수 없습니다",
        "니모닉을 백업했습니다",
    ]
    wipe_device_success = "기기 데이터 삭제 완료\n기기를 재시작하는 중..."
    bluetooth_pair = "기기에 페어링 코드를 입력하세요"
    bluetooth_pair_failed = "블루투스 페어링 실패"
    path = "파생 경로:"
    chain_id = "체인 ID:"
    send = "전송"
    to = "수신처"
    amount = "금액"
    from_ = "발신처"
    receiver = "수신자"
    fee = "수수료"
    max_fee = "최대 수수료"
    max_priority_fee_per_gas = "최대 우선순위 수수료"
    max_fee_per_gas = "Gas 당 최대 수수료"
    max_gas_limit = "Giới hạn khí tối đa:"
    gas_unit_price = "Giá đơn vị khí:"
    gas_price = "가스 가격"
    total = "총액"
    do_sign_this_transaction = "이 {} 거래에 서명하시겠습니까?"
    transaction_signed = "거래 서명 완료"
    address = '주소:'
    public_key = "공개키:"
    xpub = "XPub:"
    unknown_tx_type = "알 수 없는 거래 유형입니다. 입력 데이터를 확인하세요"
    unknown_function = "알 수 없는 함수:"
    use_app_scan_this_signature = "지갑 앱으로 서명 결과를 스캔하세요"
    internal_error = "내부 오류"
    tap_switch_to_airgap = "QR 코드를 탭하여 에어갭 주소 표시로 전환"
    tap_switch_to_receive = "QR 코드를 탭하여 수신 주소 표시로 전환"
    incorrect_pin_times_left = "PIN 오류, {}회 남았습니다"
    incorrect_pin_last_time = "PIN 오류, 마지막 기회입니다"
    wrong_pin = "잘못된 PIN 코드입니다"
    seedless = "시드 없음"
    backup_failed = "백업 실패!"
    need_backup = "백업 필요!"
    pin_not_set = "PIN 미설정!"
    experimental_mode = "실험 모드"
    pin_change_success = "PIN 코드 변경 성공"
    pin_enable_success = "PIN 코드 활성화 성공"
    pin_disable_success = "PIN 코드 비활성화 성공"
    contract = "컨트랙트:"
    new_contract = "새 컨트랙트?"
    bytes_ = "{} 바이트"
    message = "메시지:"
    no_message = "메시지 없음"
    contains_x_key = "{} 키 포함"
    array_of_x_type = "{} {} 배열"
    do_sign_712_typed_data = "이 구조화 데이터 거래에 서명하시겠습니까?"
    do_sign_typed_hash = "이 구조화 해시 거래에 서명하시겠습니까?"
    domain_hash = "도메인 해시:"
    message_hash = "메시지 해시:"
    switch_to_update_mode = "업데이트 모드로 전환"
    switch_to_boardloader = "보드 로더 모드로 전환"
    list_credentials = "이 기기에 저장된 인증서 정보를 내보내시겠습니까?"
    coinjoin_at_x = "다음 Coinjoin 거래에 참여하시겠습니까?\n{}"
    signature_is_valid = "서명 유효"
    signature_is_invalid = "서명 무효"
    u2f_already_registered = "U2F 이미 등록됨"
    u2f_not_registered = "U2F 미등록"
    fido2_already_registered_x = "FIDO2 {} 등록 완료"
    fido2_verify_user = "FIDO2 사용자 인증"
    device_already_registered_x = "기기 {} 등록 완료"
    device_verify_user = "기기 사용자 인증"
    fee_is_unexpectedly_high = "수수료 과다"
    too_many_change_outputs = "잔돈 출력이 너무 많습니다"
    change_count = "잔돈 개수"
    locktime_will_have_no_effect = "잠금 시간 영향 없음"
    confirm_locktime_for_this_transaction = "이 거래의 잠금 시간 확인"
    block_height = "블록 높이"
    time = "시간"
    amount_increased = "금액 증가"
    amount_decreased = "금액 감소"
    fee_unchanged = "수수료 변경 없음"
    fee_increased = "수수료 증가"
    fee_decreased = "수수료 감소"
    your_spend = "사용 금액"
    change_label_to_x = "라벨을 {}로 변경"
    enable_passphrase = "Passphrase 암호화를 활성화하시겠습니까?"
    disable_passphrase = "Passphrase 암호화를 비활성화하시겠습니까?"
    enable_passphrase_always = "항상 이 기기에서 Passphrase를 입력하시겠습니까?"
    revoke_enable_passphrase_always = "기기에서 항상 Passphrase 입력 설정을 취소하시겠습니까?"
    auto_lock_x = "{} 후 자동 잠금을 설정하시겠습니까?"
    enable_safety_checks = "엄격한 안전 검사를 실행하시겠습니까? 더 포괄적인 보안이 제공됩니다."
    disable_safety_checks = "안전 검사를 비활성화하시겠습니까? 계속하기 전에 잠재적 보안 위험을 숙지하세요."
    enable_experiment_mode = "실험 모드를 활성화하시겠습니까?"
    set_as_homescreen = "홈 화면을 변경하시겠습니까?"
    replace_homescreen = "홈 화면을 교체하시겠습니까? 가장 오래된 배경화면이 삭제됩니다."
    confirm_replace_wallpaper = "홈 화면의 배경화면을 교체하시겠습니까?"
    get_next_u2f_counter = "다음 U2F 카운터를 가져오시겠습니까?"
    set_u2f_counter_x = "U2F 카운터를 {}로 설정하시겠습니까?"
    confirm_entropy = "엔트로피를 추출하시겠습니까? 진행 전에 작업 내용을 반드시 확인하세요!"
    bandwidth = "대역폭"
    energy = "에너지"
    sender = "송신자"
    recipient = "수신자"
    resource = "자원"
    frozen_balance = "동결 금액"
    unfrozen_balance = "해제 금액"
    delegated_balance = "위임 금액"
    undelegated_balance = "위임 취소 금액"
    you_are_freezing = "자산을 동결하는 중입니다"
    you_are_unfreezing = "자산을 해제하는 중입니다"
    you_are_delegating = "자산을 위임하는 중입니다"
    you_are_undelegating = "위임을 취소하는 중입니다"
    duration = "지속 시간"
    lock = "잠금"
    unlock = "해제"
    all = "전체"
    source = "출처"
    tip = "팁"
    keep_alive = "연결 유지"
    invalid_ur = "지원되지 않는 QR 코드 유형입니다. 다시 시도하세요"
    sequence_number = "일련번호"
    expiration_time = "만료 시간"
    argument_x = "인수 #{}"
    low_power_message = "배터리 {}% 남음\n충전이 필요합니다"
    collect_nft = "이 NFT를 수집하시겠습니까?"
    replace_nft = "이 NFT를 수집하시겠습니까? 저장 공간이 제한되어 있어 가장 오래된 업로드된 NFT가 제거됩니다."
    verify_device = "HyperMate 서버로 기기를 인증하시겠습니까? 확인을 탭하여 기기가 정품이며 변조되지 않았는지 확인하세요."
    update_bootloader = "부트로더를 업데이트하시겠습니까?"
    update_resource = "기기 리소스를 업데이트하시겠습니까?"

class Tip:
    swipe_down_to_close = "닫으려면 아래로 스와이프하세요"

class Button:
    done = "완료"
    ok = "확인"
    confirm = "승인"
    reject = "거절"
    next = "다음"
    cancel = "취소"
    redo = "다시 생성"
    continue_ = "계속"
    try_again = "재시도"
    power_off = "전원 끄기"
    restart = "재시작"
    hold = "길게 누르기"
    address = "주소"
    qr_code = "QR 코드"
    view_detail = "상세 보기"
    hold_to_sign = "서명하려면 길게 누르세요"
    hold_to_wipe = "초기화하려면 길게 누르세요"
    receive = "수신 주소"
    airgap = "에어갭"
    sign = "서명"
    verify = "검증"
    view_full_array = "전체 배열 보기"
    view_full_struct = "전체 구조 보기"
    view_full_message = "전체 메시지 보기"
    view_data = "데이터 보기"
    view_more = "더 보기"

class WalletSecurity:
    header = "종이에 니모닉을 적어 안전한 곳에 보관하세요"
    tips = [
        {
            "level": "info",
            "msgs": [
                "#00001F  니모닉은 안전하게 보관해야 합니다#",
                "#18794E * 은행 금고에 보관#",
                "#18794E * 안전 금고에 보관#",
                "#18794E * 여러 비밀 장소에 분산 보관#",
            ]
        },
        {
            "level": "warning",
            "msgs": [
                "#00001F 반드시 주의하세요#",
                "#CD2B31 * 니모닉 백업을 꼭 기억하세요#",
                "#CD2B31 * 분실하지 마세요#",
                "#CD2B31 * 타인에게 알려주지 마세요#",
                "#CD2B31 * 인터넷에 저장하지 마세요#",
                "#CD2B31 * 컴퓨터에 저장하지 마세요#",
            ]
        },
    ]

class MnemonicSecurity:
    header = "니모닉은 지갑 자산을 복구하기 위한 단어 조합입니다. 니모닉을 소유한다는 것은 자산을 사용할 수 있다는 의미이므로 안전하게 보관하세요."

    tips = [
        "1. 주변 환경이 안전한지 확인하고, 주변에 다른 사람이나 카메라가 없는지 확인하세요",
        "2. 단어의 정확한 순서대로 니모닉을 백업하고, 절대 누구와도 공유하지 마세요",
        "3. 안전한 장소에 오프라인으로 보관하세요. 전자 방식으로 백업하거나 인터넷에 업로드하지 마세요",
    ]

class PinSecurity:
    header = "PIN 코드는 기기 접근을 위한 비밀번호로, 현재 기기의 접근 권한을 부여합니다. 아래 안내에 따라 올바르게 사용하세요."
    tips = [
        "1. PIN 설정/입력 시 주변 안전 확인 - 주변에 타인이나 카메라 없는지 확인",
        "2. 보안 강한 4~16자리 PIN 설정 (연속/반복 숫자 사용 금지)",
        "3. PIN 오류 10회 시 기기 초기화 조치됨",
        "4. PIN 코드 절대 타인과 공유 금지",
    ]

class Solana:
    ata_reciver = "수신자(연동 토큰 계정)"
    ata_sender = "송신자(연동 토큰 계정)"
    source_owner = "거래 서명자"
    fee_payer = "수수료 납부자"
