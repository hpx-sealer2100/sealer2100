class App:
    account = "賬戶"
    connect_app = "連接App錢包"
    backup = "備份助記詞"
    scan = "掃描"
    nft = "NFT 陳列室"
    guide = "使用說明"
    security = "安全"
    setting = "設定"


#### Setting App
class Setting:
    check_security = "安全檢查"
    check_mnemonic = "核對助記詞"
    remove_pin = "删除 PIN"
    change_pin = "更改 PIN"
    enable_pin = "啓用 PIN"
    remove_iris = "删除虹膜"
    change_iris = "更改虹膜"
    regist_iris = "啓用虹膜"
    reset = "重置設備"
    defi_Lock ="DeFi 鎖"
    pin_management="PIN 管理"
    iris_management="虹膜管理"
    pin_keyboard="PIN 鍵盤"
    authentication ="設備認證"
    screen_wake_up = "螢幕喚醒"
    dynamic_effects = "動態效果"
    developer_options ="開發者選項"
    about_device ="關於設備"
    wallet = "錢包"
    security = "安全"
    home_screen ="主螢幕"
    airgap_mode ="Air Gap 模式"
    general =  "通用"
    verify_device ="驗證設備"
    bluetooth = "藍牙"
    qr_code = "二維碼"
    language = "語言"
    vibration = "震動與觸摸回饋"
    brightness = "亮度"
    auto_lock = "自動鎖定"
    auto_shutdown = "自動關機"
    auto_lock_and_shutdown = "自動鎖定/關機"
    animation = "過場動畫"
    wallpaper = "壁紙"
    power_off = "關機"
    restart = "重新啟動"
    input_on_device = "在設備上輸入"

#### Security App
class Security:
    change_pin = "更改 PIN"
    backup_mnemonic = "備份助記詞"
    check_mnemonic = "檢查助記詞"
    wipe_device = "抹掉裝置"

#### guide App
class Guide:
    quickstart = "Sealer2100 快速上手"
    device_on_and_off = "設備開機和關機"
    what_mnemonic = "什麼是助記詞？"
    enable_pin_protection = "啟用 PIN 保護"
    enable_iris_protection = "啟用虹膜保護"
    how_work = "硬體錢包的運作原理"
    passphrase = "Passphrase 與隱藏錢包"
    defi_lock = "DeFi 鎖"
    need_help = "需要幫助？"

    device_on_and_off_desc = "按住電源鍵操作開機或關機。"
    what_mnemonic_desc = "助記詞是由私鑰轉換而來的便於記憶的可讀格式，是恢復你全部加密資產的唯一關鍵。務必妥善保管，切勿洩漏給任何人。"
    enable_pin_protection_desc = "透過系統級安全防護機制，設定一個高強度的 PIN 碼，為您的加密資產提供全方位保護。"
    enable_iris_protection_desc = "完成虹膜註冊後，您即可透過虹膜來解鎖設備。需要時，只需使用前置虹膜模組掃描已註冊的眼睛即可。"
    how_work_desc = "Sealer2100 將助記詞離線加密存儲在安全晶片中，只能透過藍牙或 Airgap 與 HPX 應用程式進行連接和互動。"
    passphrase_desc = "Passphrase 可理解為硬體錢包的擴展助記詞（助記詞+1）。透過輸入不同的 Passphrase，您可以進入對應的隱藏錢包。"
    defi_lock_desc = "一鍵關閉 DeFi 交易（保留轉帳），防禦合約漏洞與釣魚風險，為您的資產提供機構級安全防護。"


class Nft:
    nft_item ="{} item"
    nft_items ="{} items"
class Title:
    word_input ="輸入單字 #{}"
    iris_management="虹膜管理"
    airgap_mode_close="關閉僅支援 Air Gap"
    airgap_mode_open="開啟僅支援 Air Gap"
    connect_hpx_wallet ="連接到HPX錢包"
    connect_metamask_wallet ="連接到MetaMask錢包"
    connect_okx_wallet ="連接到歐易 Web3 錢包"
    learn_more = "了解更多"
    blue_connect = "藍牙連接说明"
    qr_connect = "二維碼鏈接"
    start_setting = "開始設定"
    link_wallet = "連接錢包"
    download_app ="下載App"
    word_check = "單字 #{}"
    nearing_completion="即將完成"
    start_backup = "開始備份"
    enable_protection = '啟用保護'
    prepare_create = '準備創建'
    prepare_recover = '準備導入'
    prepare_check='準備核對'
    activate_device = '激活設備'
    enter_old_pin = "輸入舊 PIN"
    enter_new_pin = "輸入新 PIN"
    enter_pin = "輸入 PIN"
    enter_pin_again = "再次輸入 PIN"
    select_language = "語言"
    create_wallet = "創建新錢包"
    wallet = "錢包"
    import_wallet = "導入钱包"
    restore_wallet = "恢復錢包"
    wallet_is_ready = "錢包已就緒"
    select_word_count = "選擇單詞數量"
    wallet_security = "錢包安全"
    pin_security = "PIN 安全提示"
    mnemonic_security = "助記詞安全提示"
    backup_mnemonic = "助記詞"
    enter_mnemonic = "輸入助記詞"
    check_mnemonic = "檢查助記詞"
    success = "成功"
    operate_success = "操作成功"
    theme_success = "主題切換成功"
    warning = "警告"
    error = "錯誤"
    verified = "助記詞備份完成"
    invalid_mnemonic = "無效的助記詞"
    terminate_mnemonic ="終止核對"
    pin_not_match = "不匹配"
    check_recovery_mnemonic = "檢查恢復助記詞"
    mnemonic_not_match = "助記詞不匹配"
    power_off = "關機"
    restart = "重新啟動"
    change_language = "更改語言"
    wipe_device = "抹掉裝置"
    bluetooth_pairing = "藍牙配對"
    address="{} 地址"
    public_key = "{} 公鑰"
    xpub = "{} XPub"
    transaction = "{} 交易"
    transaction_detail = "交易詳情"
    confirm_transaction = "確認交易"
    confirm_message = "確認訊息"
    signature = "簽名結果"
    wrong_pin = "PIN 錯誤"
    pin_changed = "PIN 已修改"
    pin_enabled = "PIN 已啟用"
    pin_disabled = "PIN 已停用"
    pin_removed = "PIN 已刪除"
    unknown_token = "未知代幣"
    view_data = "查看數據"
    sign_message = "{} 訊息簽名"
    eip712_sign_message = "签署 {} 讯息"
    verify_message = "{} 訊息驗簽"
    typed_data = "{} 結構化數據"
    typed_hash = "{} 結構化哈希"
    system_update = "系統升級"
    entering_boardloader = "Entering Boardloader"
    remove_credential = "刪除憑證"
    list_credentials = "列出憑證"
    authorize_coinjoin = "授權CoinJoin"
    multisig_address_m_of_n = "{} 多重簽名地址\n({} of {})"
    u2f_register = "U2F 註冊"
    u2f_unregister = "U2F 註銷"
    u2f_authenticate = "U2F 認證"
    fido2_register = "FIDO2 註冊"
    fido2_unregister = "FIDO2 註銷"
    fido2_authenticate = "FIDO2 認證"
    finalize_transaction = "完成交易"
    meld_transaction = "融合交易"
    update_transaction = "更新交易"
    high_fee = "高費用"
    fee_is_high = "手續費過高"
    confirm_locktime = "確認鎖定時間"
    view_transaction = "查看交易"
    x_confirm_payment = "{} 確認付款"
    confirm_replacement = "確認替換交易"
    x_transaction = "{} 交易"
    x_joint_transaction = "{} 聯合交易"
    change_label = "修改裝置名稱"
    enable_passphrase = "啟用 Passphrase"
    disable_passphrase = "停用 Passphrase"
    passphrase_source = "Passphrase 輸入設定"
    enter_passphrase ="输入 Passphrase"
    enable_safety_checks ="啟用安全檢查"
    disable_safety_checks ="停用安全檢查"
    experiment_mode = "實驗模式"
    set_as_homescreen = "設為主屏幕"
    collect_nft = "收藏此 NFT"
    delete_nft = "刪除 NFT"
    get_next_u2f_counter = "獲取U2F計數器"
    set_u2f_counter = "設定U2F計數器"
    encrypt_value = "加密數據"
    decrypt_value = "解密數據"
    confirm_entropy = "導出熵"
    memo = "備註"
    import_credential = "導入憑證"
    export_credential = "導出憑證"
    asset = "資產"
    unimplemented = "未實現"
    invalid_data="無效的數據格式"
    low_power = "電量低"
    verify_device = "驗證裝置"
    update_bootloader = "更新引導加載程序"
    update_resource = "更新資源"
    directions = "方向"
    send_amount = "send\n{}"
    reset_device ="重置此設備"
    device_protection_verification_failed = "驗證未通過"
    correct = "正確"
    spl_create_ata = "创建代币账户"
    select_derivation_path = "選擇派生路徑"
    approve_confirm = "授權確認"
    contract_call = "合約調用"
    power_off = "確定要關機嗎？"
class Text:
    note = "注意"
    export_signed_transactions ="導出已簽署的交易"
    export_signed_transactions_desc ="返回APP，並掃描下方的二維碼"
    detail ="詳情"
    transaction_send = "發送 {} {}"
    transaction_fee = "費用"
    transaction_direction = "转出"
    transaction_from = "發送方"
    transaction_to = "接收方"
    gas_limit = "交易費上限"
    gas_fee_cap = "單位Gas的交易費上限"
    gas_premium = "單位Gas的優先費用"
    wallet_not_match ="錢包不匹配"
    wallet_not_match_desc ="目前硬體錢包和app中選擇的錢包不匹配，請檢查是否Passphrase輸入錯誤或錢包選擇錯誤"
    transaction_signed ="交易已簽署"
    transaction_signed_desc ="您的交易已簽署"
    invalid_transaction ="無效的交易"
    invalid_transaction_desc ="無效的交易數據，請重試"
    scan_app="掃描 App 上顯示的二維碼"
    use_passphrase ="使用此Passphrase？"
    use_passphrase_desc ="請檢查已輸入的短語，確保其完全正確。"
    enter_mnemonic_desc="請按照順序輸入單字，確保編號與您的助記詞備份完全匹配。"
    limit_error ="錯誤已達上限"
    limit_error_desc ="PIN 碼錯誤次數已達上限，裝置即將在 #28C600 5# 秒後重置"
    backup_completed="備份完成"
    backup_completed_desc="您已成功備份助記詞"
    MetaCard_pro="Sealer MetaCard"
    passphrase_disable ="Passphrase已停用，目前僅支援添加標準錢包"
    passphrase_disable_desc ="要停用Passphrase（隱藏錢包）功能嗎？"
    passphrase_enable ="Passphrase 功能已啟用。每次連接 Sealer2100 應用程式時，裝置均需輸入 Passphrase，請牢記您設定的Passphrase"
    passphrase_enable_desc ="啟用後，可在App端根據不同的Passphrase添加出不同的隱藏錢包"
    remove_pin_desc = "請確認是否要刪除 PIN 碼，僅保留虹膜辨識？"
    enable_pin_ok ="你已成功啟用 PIN 。"
    enable_pin_titles =[
        "高強度 PIN 碼",
        "妥善保存"
    ]
    enable_pin_tips =[
        "請用高強度的 PIN 碼，以防止錢包被未經授權的操作訪問。",
        "妥善保存 PIN 碼，確保與助記詞分開存放。"
    ]
    keyboard_feedback="鍵盤觸感回饋"
    keyboard_feedback_desc ="與系統互動時提供振動回饋"
    enable_pin ="啟用 PIN 碼保護"
    set_pin_length ="設定一個長度為 #28C600 4# 到 #28C600 16# 位的 PIN 碼，用來保護您的錢包。"
    reset_device_desc ="此操作將清空設備的內部儲存空間以及安全晶片  (SE) 中的所有資料。在繼續之前，請知曉以下事項："
    reset_device_check = [
        "重置後，此裝置上你的助記詞將會永久刪除。",
        "您仍然可以透過助記詞備份找回您的錢包和資產。"
    ]
    reset_warning = "警告\n恢复硬件设备至出厂设置,此操作将清除设备上的所有数据。"
    defi_disable ="DeFi 交易被禁止"
    defi_disable_desc ="DeFi鎖定開啟期間，將禁止所有除轉帳以外的交易行為。"
    defi_lock_desc="DeFi 鎖定開啟後，將禁止所有除轉帳以外的交易行為"
    scan_learn_more_verify_device="掃碼了解更多驗證方法"
    verify_device_desc = [
        {
            "title": "下載 HPX Wallet",
            "message": "從以下地址下載 HPX Wallet App，行動端：",
        },
        {
            "title": "驗證裝置",
            "message": "打開 HPX Wallet App 進入 Web3 並連接您的裝置以創建錢包。裝置驗證將自動進行。",
        },
    ]
    check_security_enable_desc ="啟用安全檢查後，將禁止衍生不符合 BIP-39 協議的位址，並避免執行潛在安全風險或費用過高的交易。"
    scan_learn_more=  "掃碼了解更多教學"
    system_update_desc = [
        {
            "title": "下載 HPX Wallet",
            "message": "從以下地址下載 HPX Wallet App，行動端：",
        },
        {
            "title": "開始更新",
            "message": "打開 HPX Wallet App 應用，前往「Web3 錢包」，選擇「導入錢包」>「連接硬體錢包」>「Sealer2100」，然後進入錢包管理，選擇當前硬體後進入管理選項的「系統更新」。",
        }
    ]

    download_app = "前往下載 HPX 應用程式："
    serial_number = '序列號'
    model = '型號'
    bluetooth_name = "藍牙名稱"
    system_version = '系统版本'
    bluetooth_version = '藍牙'
    bootloader="Bootloader"
    fcc="FCC"
    compatible="相容 Trezor"
    system_update="系統更新"
    set_as_homescreen_desc = "確定要將目前 NFT 設定成為主畫面嗎?"
    delete_nft_desc = "確定要刪除目前 NFT 吗？\n\n刪除 NFT 操作仅從設備中移除 NFT，NFT 并不會從鏈接中刪除。您可以在隨時重新添加 NFT。"
    enable_pin="啟用 PIN 碼"
    enable_pin_desc="您目前暫未啟用 PIN 碼，是否啟動 PIN 碼，開啟雙重驗證保護"
    iris_operation_starting = "等待虹膜啓動"
    iris_start_timeout = "虹膜啓動超"
    iris_eye_focus = "將您的眼睛對準圓圈內"
    iris_regist = "虹膜註冊"
    iris_match = "虹膜驗證"
    iris_removing_old = "正在移除旧虹膜"
    iris_changed="虹膜已更改"
    iris_changed_ok="您已成功更改虹膜。"
    iris_changed_failed="更改虹膜失敗。"
    iris_removed = "虹膜已移除"
    iris_removed_ok = "您已成功移除虹膜。"
    iris_removed_failed = "移除虹膜失敗。"
    front_of_screen ="請位於螢幕正前方"
    near_of_screen="請您適當靠近一些"
    far_of_screen="請您適當後退一些"
    iris_regist_suggestion_distance_close = "請後退一些，於 20–30cm 內進行註冊"
    iris_regist_suggestion_distance_far   = "請靠近一些，於 20–30cm 內進行註冊"
    iris_match_suggestion_distance_close  = "請後退一些，於 20–30cm 內進行識別"
    iris_match_suggestion_distance_far    = "請靠近一些，於 20–30cm 內進行識別"
    iris_suggestion_registing="正在註冊，请勿移动"
    iris_suggestion_matching="正在識別，請勿移動"
    iris_operation_matching = "识别中..."
    iris_operation_registing = "註冊中..."
    iris_operation_match_done = "識別成功"
    iris_operation_regist_done = "註冊成功"
    iris_current_distance ="目前距離 "
    new_iris_registration="新虹膜註冊"
    new_iris_registration_desc="開始錄入新的虹膜，用於後續的安全驗證。"
    change_iris="更改虹膜"
    remove_iris="移除虹膜"
    remove_iris_desc= "刪除虹膜驗證將顯著降低您的裝置安全等級，僅保留 PIN 碼驗證可能無法提供足夠保護，建議謹慎操作"
    iris_verification ="原虹膜驗證"
    iris_verification_desc ="驗證您裝置上原有的虹膜訊息"
    change_iris_desc="是否要刪除目前已註冊虹膜資訊後重新錄入新的虹膜？"
    register_iris_ok ="虹膜輸入成功"
    register_iris_error ="虹膜輸入失敗"
    verify_iris_error ="虹膜驗證失敗"
    verify_iris_error_desc ="虹膜驗證失敗，請重試"
    register_iris_timed_out ="虹膜錄入超時，請重試"
    iris_ok_desc ="已成功添加虹膜，可用於安全識別驗證."
    register_iris_desc = "虹膜驗證是一種基於您獨特虹膜特徵的生物辨識技術，提供最高級別的安全保障，確保只有您能存取您的錢包。"
    iris_entry_desc = "因爲此類物件在註冊虹膜時可能因反光或遮擋導致虹膜資料不完整，進而造成設備後續無法解鎖。"
    register_iris="註冊虹膜"
    register_iris_desc="目前暫未啟用更高安全性的虹膜驗證，是否立即註冊虹膜，開啟雙重驗證保護?"
    suggest_enable_iris = "為提升安全性，建議同時錄入虹膜資訊，開啟 PIN 碼和虹膜雙重保護。"
    iris_duplicate = "虹膜已註冊"
    iris_duplicate_dsc = "當前虹膜已經註冊，不需要進行更改"
    iris_note = "請再次確認您目前沒有配戴任何眼鏡、隱形眼鏡或其他遮擋眼睛的物件。"
    random= "隨機"
    default ="預設"
    pin_random="PIN 鍵盤上的數字隨機排列"
    pin_default="PIN 鍵盤上的數字依序排列"
    pin_changed ="你已成功更改 PIN"
    airgap_mode_desc ="開啟僅支援 Air Gap 後，將停用裝置透過藍牙連線 App 功能。"
    airgap_mode = "僅支援 Air Gap"
    screen_wake_up_tap="輕點喚醒"
    screen_wake_up_button="按鍵喚醒"
    auto_shutdown_desc = "在設備熄滅狀態下，若連續 {}無操作，系統將自動關機"
    auto_shutdown_never_desc =  "在設備熄滅狀態下，系統不會自動關機"
    screen_wake_up_tap_desc ="目前設定為螢幕鎖定狀態，輕觸螢幕即可喚醒。"
    screen_wake_up_button_desc ="目前設定為螢幕鎖定狀態，按下電源鍵即可喚醒螢幕"
    dynamic_effects_disabled_desc = "已停用使用者介面的動態效果"
    dynamic_effects_allow_desc = "允許用戶界面中的動態效果"
    vibration_desc ="與系統互動時提供振動回饋"
    auto_lock_desc = "在連續 {}無操作後，螢幕會自動熄滅並鎖定。"
    auto_lock_never_desc = "不自动熄屏。"
    metamask_connect_desc ="ETH和EVM網絡"

    blue_connect_hpx_wallet = [
        {
            "title": "下載 HPX Wallet",
            "message": "從以下地址下載 HPX Wallet App，行動端：",
        },
        {
            "title": "通過藍牙連接",
            "message": [
                "• 打開手機藍牙，並將硬體錢包靠近手機。",
                "• 確認硬體未開啟「僅 AirGap 模式」，該模式在設備初始化時預設為關閉。",
                "• 在 App 中選擇搜尋到的設備名稱以完成連接。",
                "#FFFFFF 提示：#若已手動開啟「僅 AirGap 模式」，藍牙功能將被禁用，需在硬體設定中關閉該模式後才能透過藍牙連接 App。",
            ]
        },
        {
            "title": "配對裝置",
            "message": "在 HPX Wallet 應用中輸入 Sealer2100 的配對碼，驗證後選擇「開始」。"
        }
    ]

    blue_connect_okx_wallet = [
        {
            "title": "訪問錢包",
            "message": "首次打開 OKX 行動應用，前往「Web3 錢包」，選擇「導入錢包」>「連接硬體錢包」>「Sealer2100」",
        },
        {
            "title": "通過藍牙連接",
            "message": "打開兩個裝置的藍牙。保持錢包靠近手機，點擊監測到的 Sealer2100 裝置名稱。"
        },
        {
            "title": "導入錢包帳戶",
            "message": "完成配對後輸入 PIN 碼或虹膜識別，等待錢包列表加載後導入所需帳戶。"
        }
    ]

    hpx_wallet = "HPX 錢包"
    okx_wallet = "歐易 Web3 錢包"
    in_connection ="連接中..."
    in_connection_desc ="將 Sealer MetaCard 和設備保持在一起，知道傳輸完成。"
    mismatch ="不匹配"
    mismatch_desc ="您輸入的 Sealer MetaCard PIN碼不匹配，請重試。"
    need_help ="需要幫助？"
    need_help_desc ="若有問題需要解答，請前往 HPX 幫助中心尋求幫助。"
    swipe_up_to_apps ="向上滑動查看應用程序"
    processing = "處理中，請稍候..."
    signing = "正在構建交易..."
    loading = "正在載入交易..."
    connect_wallet_1 = "點擊「連接硬體錢包」。"
    connect_wallet_2 = "連接裝置「{}」。"
    connect_wallet_3 = "按照引導提示完成操作後，您硬體中的錢包帳戶將恢復至 HPX Web3 錢包列表中。"
    word_error = "單詞錯誤"
    word_error_desc = "單詞輸入有誤，請核對您的備份後再嘗試。"
    verification_passed = "驗證通過"
    verification_passed_desc = "助記詞驗證已完成。"
    correct = "正確！"
    incorrect = "錯誤！"
    backup_title = [
        "妥善保管",
        "避免拍照",
        "保持安全"
    ]
    backup_check = [
        "助記詞的遺失將導致您無法找回錢包內的所有資產，請務必妥善保管。",
        "助記詞不得以任何方式拍照、生成數字副本或通過網絡上傳，以避免泄露風險。",
        "請將助記詞記錄並存放在安全的地方，切勿與他人分享或轉發。"
    ]
    recommend = "推薦"
    please_wait = "請稍等..."
    my_wallet = "我的錢包"
    tap_to_unlock = "請點擊螢幕解鎖裝置"
    swipe_to_view_app = "向上滑動以查看應用程式"
    unlocking = "正在解鎖裝置..."
    str_words = "{} 個單詞"
    backup_manual = "手動寫下助記詞並存放在安全的地方"
    check_manual = "按順序依次點擊下面的單詞"
    backup_verified = "您已完成助記詞的備份，請妥善保存，不要與任何人分享"
    backup_invalid = "您輸入的助記詞不正確，請檢查備份的助記詞，再次嘗試"
    pin_not_match = "兩次的 PIN 碼輸入不一致，請重新輸入。"
    pin_error = "輸入的 PIN 不正確"
    wiping_device = "正在清除裝置數據..."
    create_wallet = "生成一組新的助記詞，建立新錢包"
    restore_wallet = "從您備份的助記詞恢復錢包"
    restore_mnemonic_match = "您的助記詞匹配，您的助記詞備份正確"
    restore_success = "恢復錢包成功"
    create_success = "您的助記詞已成功備份，錢包已建立"
    check_recovery_mnemonic = "請檢查您的助記詞，確認是否完全匹配"
    invalid_recovery_mnemonic = "輸入的助記詞無效，請核對您的備份後再嘗試。"
    valid_recovery_mnemonic = "輸入的助記詞已完全匹配，您的備份無誤。"
    check_recovery_not_match = "您輸入的助記詞是有效的，但是和裝置中的助記詞不匹配。"
    shutting_down = "正在關機..."
    restarting = "正在重新啟動..."
    never = "從不"
    second = "秒"
    seconds = "秒"
    minute = "分鐘"
    minutes = "分鐘"
    changing_language = "您正在更改語言\n應用此設定將重新啟動裝置"
    change_pin = "設定一個長度為 #28c600 4# 至 #28c600 16# 位的 PIN 碼，保護您的裝置"
    wipe_device = "將裝置恢復到出廠狀態。\n警告： 這將從您的裝置中抹掉所有數據。"
    wipe_device_check = [
        "抹掉裝置會清除所有數據",
        "數據將無法恢復",
        "已經備份了助記詞",
    ]
    wipe_device_success = "裝置已成功清除數據\n 請重啟裝置 ..."
    bluetooth_pair = "請在您的裝置上輸入配對碼"
    bluetooth_pair_failed = "藍牙配對失敗"
    path = "派生路徑:"
    chain_id = "Chain ID:"
    send = "傳送"
    to = "至"
    amount = "金額"
    from_ = "來自"
    receiver = "接收者"
    fee = "手續費"
    max_fee = "交易費上限"
    max_priority_fee_per_gas = "最大優先級手續費"
    max_fee_per_gas = "單位Gas的交易費上限"
    max_gas_limit = "最大燃料限制:"
    gas_unit_price = "燃料單價:"
    gas_price = "燃料價格"
    total = "總金額"
    do_sign_this_transaction = "是否簽名這筆{}交易"
    transaction_signed = "交易已簽名"
    address = '地址:'
    public_key = "公鑰:"
    xpub = "XPub:"
    unknown_tx_type = "未知交易類型，請檢查輸入數據"
    unknown_function = "未知函數:"
    use_app_scan_this_signature = "請使用錢包 APP 掃描簽名結果"
    internal_error = "內部錯誤"
    tap_switch_to_airgap = "點擊二維碼切換為展示Airgap地址"
    tap_switch_to_receive = "點擊二維碼切換為展示錢包收款地址"
    incorrect_pin_times_left = "PIN 不正確，剩餘 {} 次重試機會"
    incorrect_pin_last_time = "PIN 不正確，還有最後一次機會"
    wrong_pin = "輸入的 PIN 碼不正確"
    seedless = "缺少種子"
    backup_failed = "備份失敗！"
    need_backup = "需要備份！"
    pin_not_set = "沒有設定 PIN！"
    experimental_mode = "實驗模式"
    pin_change_success = "PIN 碼已成功更改。"
    pin_enable_success = "PIN 碼已成功啟用。"
    pin_disable_success = "PIN 碼已成功停用。"
    pin_remove_success = "PIN 码已成功移除。"
    contract = "合約:"
    new_contract = "新合約?"
    bytes_ = "{} 位元組"
    message = "訊息:"
    no_message = "不包含訊息"
    contains_x_key = "包含 {} 鍵"
    array_of_x_type = "陣列類型 {} {}"
    do_sign_712_typed_data = "是否簽署這筆結構化數據交易？"
    do_sign_typed_hash = "是否簽署這筆結構化哈希交易？"
    domain_hash = "Domain 哈希:"
    message_hash = "訊息 哈希:"
    switch_to_update_mode = "切換到更新模式"
    switch_to_boardloader = "Switch to board loader mode"
    list_credentials = "是否要導出有關存儲在此裝置上的憑證訊息？"
    coinjoin_at_x = "是否要參加下面的Coinjoin交易:\n{}"
    signature_is_valid = "簽名有效"
    signature_is_invalid = "簽名無效"
    u2f_already_registered = "U2F 已經註冊"
    u2f_not_registered = "U2F 未註冊"
    fido2_already_registered_x = "FIDO2 已註冊 {}"
    fido2_verify_user = "FIDO2 驗證用戶"
    device_already_registered_x = "裝置已註冊 {}"
    device_verify_user = "裝置驗證用戶"
    fee_is_unexpectedly_high = "手續費過高"
    too_many_change_outputs = "找零輸出太多"
    change_count = "找零數量"
    locktime_will_have_no_effect = "鎖定時間將不會有影響"
    confirm_locktime_for_this_transaction = "確認交易的鎖定時間"
    block_height = "區塊高度"
    time = "時間"
    amount_increased = "金額增加"
    amount_decreased = "金額減少"
    fee_unchanged = "手續費未改變"
    fee_increased = "手續費增加"
    fee_decreased = "手續費減少"
    your_spend = "你的花費"
    change_label_to_x = "修改標籤為 {}"
    enable_passphrase = "要啟用 Passphrase 加密嗎？"
    disable_passphrase = "要停用 Passphrase 加密嗎？"
    enable_passphrase_always = "總是在本機輸入 Passphrase 嗎？"
    revoke_enable_passphrase_always = "要撤銷總是在本機輸入 Passphrase 的設定嗎？"
    auto_lock_x = "確定要在 {} 之後自動鎖定裝置嗎？"
    enable_safety_checks = "要執行嚴格的安全檢查嗎？這將提供更全面的安全保護。"
    disable_safety_checks = "確定要停用安全檢查嗎？繼續操作前，請知悉該行為的潛在安全風險。"
    enable_experiment_mode = "是否啟用實驗模式？"
    set_as_homescreen = "確定要更改主屏幕嗎？"
    replace_homescreen = "確定要替換主屏幕嗎？這將會刪除最早上傳的壁紙。"
    confirm_replace_wallpaper = "確定要替換主屏幕的壁紙嗎？"
    get_next_u2f_counter = "確定要獲取下一個U2F計數器嗎？"
    set_u2f_counter_x = "確定要設定U2F計數器為 {} 嗎？"
    confirm_entropy = "確定要導出熵嗎？繼續操作之前請明白你在做什麼！"
    bandwidth = "頻寬"
    energy = "能量"
    sender = "傳送方"
    recipient = "接收方"
    resource = "資源"
    frozen_balance = "凍結的金額"
    unfrozen_balance = "解凍的金額"
    delegated_balance = "委託的金額"
    undelegated_balance = "撤銷委託的金額"
    you_are_freezing = "你正在凍結資產"
    you_are_unfreezing = "你正在解凍資產"
    you_are_delegating = "你正在委託資產"
    you_are_undelegating = "你正在撤銷委託"
    duration = "持續時間"
    lock = "鎖定"
    unlock = "解鎖"
    all = "全部"
    source = "來源"
    tip = "提示"
    keep_alive = "Keep alive"
    invalid_ur = "不支援的二維碼類型，請重試"
    sequence_number = "序號"
    expiration_time = "過期時間"
    argument_x = "參數 #{}"
    low_power_message = "電量還剩餘 {}%\n請充電"
    collect_nft = "你確定要收集此 NFT 嗎？"
    replace_nft = "你想要收集此 NFT 嗎？你已達到存儲上限，這將移除最早上傳的 NFT。"
    verify_device = "你確定要使用Sealer2100伺服器驗證你的裝置嗎？點擊確認以檢查你的裝置是否為原裝且未被篡改。"
    update_bootloader = "你想更新引導加載程序嗎？"
    update_resource = "你想更新裝置資源嗎？"
    need_input_tips = "請輸入完成所有助記詞以後再點擊下一步"
    need_select_tips = "請按助記詞順序點擊選擇完成以後再點擊下一步"
    return_check_mnemonic = "查看助記詞"
    device_wakeup_by_touch_screen = "輕點喚醒"
    device_wakeup_by_click_power_button = "按鍵喚醒"
    on = "啓用"
    off = "禁用"
    input_passphrase_on_device_desc = "在需要輸入 Passphrase，在設備上輸入"
    input_passphrase_on_host_desc = "在需要輸入 Passphrase，在應用程式上輸入"
    already_input = "已提交"
    please_backup = "請立即備份您的新錢包。"

    ### iris error code
    iris_code_0 = "成功"
    iris_code_1 = "活體檢測失敗"
    iris_code_2 = "模組拒絕該命令"
    iris_code_3 = "超時失敗"
    iris_code_4 = "虹膜已註冊"
    iris_code_5 = "用戶 ID 已存在"
    iris_code_6 = "沒有錄入用戶，模組中沒有用戶"
    iris_code_7 = "用戶 ID 不存在"
    iris_code_8 = "模組中用戶數量已達最大值"
    iris_code_11 = "終止註冊 / 辨識過程"
    iris_code_12 = "參數錯誤"
    iris_code_13 = "相機打開失敗"
    iris_code_14 = "模組初始化失敗"
    iris_code_15 = "活體檢測失敗"

    ## iris notes message
    iris_note_module_status_1 = "模組啟動成功"
    iris_note_module_status_2 = "模組啟動識別"
    iris_note_module_status_3 = "模組啟動註冊"

    iris_note_regist_progress = "註冊進度：{} \t {}"

    iris_note_iris_status_0 = "虹膜正常"
    iris_note_iris_status_1 = "距離太近"
    iris_note_iris_status_2 = "距離太遠"
    iris_note_iris_status_3 = "未檢測到虹膜"

    device_protection_verification_failed_msg = "PIN 碼或虹膜不匹配，請重試。"
    device_protection_verification_left = "剩餘 {} 次重試機會"
    too_many_iris_match_count = "虹膜匹配超時次數太多了\n\n是否要抹掉設備？"
    evm_address_desc = "此地址為 EVM 通用地址，可同步管理其他 EVM 相容鏈（如 Ethereum、BNB Chain、Avalanche、Arbitrum、Optimism、Polygon 等）上的資產。"
    approve_confirm_desc ="授權後，您將允許該 dApp 轉移當前代幣資產，或為您自動執行交易。"
    approve_to = "授權至"
    allowance = "授權額度"
    contract = "合約地址"
    unlimited = "無量"

class Subtitle:
    account_qr_address ="返回 App 並掃描下方的二維碼"
    confirm_transaction = "確認要簽署該筆交易嗎？"
    terminate_mnemonic ="終止本流程會導致所有的進度都會遺失，請確認是否要終止？"
    backup="您可以從以下備份方式中進行選擇。一旦助記詞通過驗證，系統會將其作為備份內容保存"
    enter_mnemonic = "请依照备份中的编号逐项输入助记词单词，确保完全一致請依照備份中的編號逐項輸入助記詞單字，確保完全一致"
    airgap_mode_close="要關閉僅支援Air Gap模式嗎？關閉後，將允許App透過藍牙連接此裝置"
    airgap_mode_open="是否確認開啟僅支援 Air Gap 模式？啟用後，所有連接軟體錢包及簽署交易等操作都將透過掃描二維碼在離線環境中完成"
    qr_connect_wallet_desc_metamask = "打開 MetaMask 錢包掃描下方二維碼，支援 Ethereum 與 EVM 網路。"
    qr_connect_wallet_desc_hpx = "打開 HPX 錢包掃描下方二維碼，支援 Bitcoin、Solana、Tron、Ethereum 以及 EVM 網路。"
    equipment_certification ="是否確定使用 HPX 安全伺服器對您的裝置進行防偽驗證？點擊確認鍵繼續，以確保您的裝置為正品且未被竄改。"
    learn_more = "掃描二維碼，查看詳細教學"
    choose_connection_wallet ="選擇您想要連接的錢包。"
    choose_connection_method = "選擇連接方式。"
    start_setting = "将 Sealer MetaCard 贴合设备的背面，然后点击继续。"
    import_wallet = "選擇您想要的導入方式"
    nft_count = "{} 個展品"
    word_check = "請選擇正確的單詞"
    nearing_completion = "請按照提示，依次核對您備份的助記詞單詞。"
    backup_mnemonic = "按順序抄寫下方的 #00FE33 {}# 個單詞。"
    start_backup ="接下來將展示一組稱為助記詞的單詞。在此之前，請務必註意以下事項："
    wallet_is_ready = "新錢包創建成功！"
    enable_protection = "PIN 或虹膜識別是您的數字資產保險鎖。開啟後，所有敏感操作都將受到全面保護。"
    select_mnemonics = "選擇助記詞位數。"
    activate_device = "創建新的助記詞，或導入已有助記詞來恢復錢包。"
    create_wallet ="即將創建由一組新助記詞生成的標準錢包。"
    reset_wallet = "將設備還原為出廠設定。"
    iris_desc = "註冊時請您摘下眼鏡、墨鏡或隱形眼鏡等佩戴物，保證裸眼完成註冊。"
class Tip:
    swipe_down_to_close = "向下滑動以關閉"
class Button:
    back ="返回"
    signature_transaction ="簽名交易"
    enable ="啓用"
    disable = "禁用"
    delete ="删除"
    reset = "重置"
    set_home_screen="設為主螢幕"
    iris_entry="錄入虹膜"
    iris_not_now = "暫不錄入"
    to_open="去開啟"
    verify_now = "立即驗證"
    learn_more ="了解更多"
    mnemonic= "助記詞"
    metacard = "Sealer MetaCard"
    close = "關閉"
    enable_pin_iris = "同时启用 PIN 码和虹膜验证"
    enable_pin = "启用 PIN 码验证"
    enable_iris = "启用虹膜验证"
    create_wallet ="创建新钱包"
    import_wallet = "导入钱包"
    done = "完成"
    ok = "好的"
    confirm = "確定"
    reject = "拒絕"
    next = "下一步"
    cancel = "取消"
    redo = "重新生成"
    continue_ = "繼續"
    try_again = "重試"
    power_off = "關機"
    restart = "重新啟動"
    hold = "按住"
    address = "地址"
    qr_code = "二維碼"
    view_detail = "查看詳情"
    hold_to_sign = "長按簽名"
    hold_to_wipe = "長按抹掉"
    receive = "收款地址"
    airgap = "Airgap"
    sign = "簽名"
    verify = "驗證"
    view_full_array = "查看完整陣列"
    view_full_struct = "查看完整結構"
    view_full_message = "查看完整訊息"
    view_data = "查看數據"
    view_more = "查看更多"
    start = "開始"
class WalletSecurity:
    header = "在一張紙上寫下您的助記詞並將其存放在安全的地方"
    tips = [
        {
            "level": "info",
            "msgs": [
                "#00001F 助記詞需要安全存放#",
                "#18794E * 存放在銀行金庫中#",
                "#18794E * 存放在保險箱中#",
                "#18794E * 存放在多個秘密地點#",
            ]
        },
        {
            "level": "warning",
            "msgs": [
                "#00001F 一定要注意#",
                "#CD2B31 * 謹記保存的助記詞#",
                "#CD2B31 * 不要丟失#",
                "#CD2B31 * 不要告訴別人#",
                "#CD2B31 * 不要存儲在網路上#",
                "#CD2B31 * 不要存儲在電腦上#",
            ]
        },
    ]

class MnemonicSecurity:
    header = "助記詞是一組用來恢復錢包資產的短語，擁有助記詞就意味著可以使用您的資產，請妥善保管"

    tips = [
        "1. 請檢查環境安全，確保沒有旁觀者或攝像頭",
        "2. 請按短語的正確順序備份助記詞，切勿與任何人分享您的助記詞",
        "3. 請在安全的地方離線保存助記詞，切勿使用電子方式備份助記詞，切勿上傳網路",
    ]

class PinSecurity:
    # 定义类属性header，用于描述PIN码的作用和使用提示
    header = "PIN碼 是裝置訪問的密碼，用於授權訪問當前裝置。請按照以下提示正確使用 PIN 碼"
    # 定义类属性tips，用于存储PIN码使用的具体提示信息
    tips = [
        "1. 設定或錄入 PIN 時請檢查環境安全，確保沒有旁觀者或攝像頭",  # 提示1：设置或输入PIN码时请检查环境安全，确保没有旁观者或摄像头
        "2. 請設定長度為 4-16 位的高強度的 PIN 碼，避免使用連續或重複的數字",
        "3. PIN 碼的最大重試次數為10次，當錯誤10次後裝置將會被重置",
        "4. 請妥善保管 PIN 碼，不要和任何人分享您的 PIN 碼",
    ]

class Solana:
    ata_reciver = "接收者(關聯代幣帳戶)"
    ata_sender = "發送者(關聯代幣帳戶)"
    source_owner = "交易簽署者"
    fee_payer = "手續費支付者"
    associated_token_account = "新的代幣帳戶"
    owner = "擁有者"
    mint_address = "鑄幣地址"
    found_by = "資助方"
    blinding_sign = "Solana 原始消息簽名"
    blinding_sign_dsc = "允許簽署原始 Solana 消息而不進行處理或驗證。這可能會讓您面臨網路釣魚、盲簽和未經授權的批准。請謹慎使用。"

class Filecoin:
    gas_limit = "燃料用量上限"
    gas_fee_cap = "燃料單價上限"
    gas_premium = "優先費用"

class Ripple:
    destination_tag = "目的地標籤"

