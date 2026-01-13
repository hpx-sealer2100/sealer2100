class App:
    account = "账户"
    connect_app = "连接App钱包"
    backup = "备份助记词"
    scan = "扫描"
    nft = "NFT 陈列室"
    guide = "使用说明"
    security = "安全"
    setting = "设置"


#### Setting App
class Setting:
    check_security = "安全检查"
    check_mnemonic ="核对助记词"
    remove_pin = "删除 PIN"
    change_pin = "更改 PIN"
    enable_pin = "启用 PIN"
    remove_iris = "删除虹膜"
    change_iris = "更改虹膜"
    regist_iris = "启用虹膜"
    reset = "重置设备"
    defi_Lock ="DeFi锁"
    pin_management="PIN 管理"
    iris_management="虹膜管理"
    pin_keyboard="PIN 键盘"
    authentication ="设备认证"
    screen_wake_up = "屏幕唤醒"
    dynamic_effects = "动态效果"
    developer_options ="开发者选项"
    about_device ="关于设备"
    wallet = "钱包"
    security = "安全"
    home_screen ="主屏幕"
    airgap_mode ="Air Gap 模式"
    general =  "通用"
    verify_device ="验证设备"
    bluetooth = "蓝牙"
    qr_code = "二维码"
    language = "语言"
    vibration = "震动与触摸反馈"
    brightness = "亮度"
    auto_lock = "自动锁定"
    auto_shutdown = "自动关机"
    auto_lock_and_shutdown = "自动锁定/关机"
    animation = "过场动画"
    wallpaper = "壁纸"
    power_off = "关机"
    restart = "重新启动"
    input_on_device = "在设备上输入"
#### Security App
class Security:
    change_pin = "更改 PIN"
    backup_mnemonic = "备份助记词"
    check_mnemonic = "检查助记词"
    wipe_device = "抹掉设备"

#### guide App
class Guide:
    quickstart = "Sealer2100 快速上手"
    device_on_and_off = "设备开机和关机"
    what_mnemonic ="什么是助记词？"
    enable_pin_protection ="启用 PIN 保护"
    enable_iris_protection="启用虹膜保护"
    how_work = "硬件钱包的运作原理"
    passphrase = "Pasphrase与隐藏钱包"
    defi_lock ="DeFi锁"
    need_help ="需要帮助？"

    device_on_and_off_desc = "按住电源键操作开机或关机。"
    what_mnemonic_desc = "助记词是由私钥转换而来的便于记忆的可读格式，是恢复你全部加密资产的唯一关键。务必妥善保管，切勿泄漏给任何人。"
    enable_pin_protection_desc = "通过系统级安全防护机制，设置一个高强度的 PIN 码，为您的加密资产提供全方位保护。"
    enable_iris_protection_desc = "完成虹膜注册后，您即可通过虹膜来解锁设备。需要时，只需使用前置虹膜模组扫描已注册的眼睛即可。"
    how_work_desc = "Sealer2100 将助记词离线加密存储在安全芯片中，只能通过蓝牙或Airgap与HPX 应用程序进行连接和交互。"
    passphrase_desc = "Passphrase 可理解为硬件钱包的扩展助记词（助记词+1）。通过输入不同的 Passphrase，您可以进入对应的隐藏钱包。"
    defi_lock_desc = "一键关闭 DeFi 交易（保留转账），防御合约漏洞与钓鱼风险，为您的资产提供机构级安全防护。"



class Nft:
    nft_item ="{} item"
    nft_items ="{} items"
class Title:
    word_input ="输入单词 #{}"
    iris_management="虹膜管理"
    airgap_mode_close="关闭仅支持 Air Gap"
    airgap_mode_open="开启仅支持 Air Gap"
    connect_hpx_wallet ="连接到HPX钱包"
    connect_metamask_wallet ="连接到MetaMask钱包"
    connect_okx_wallet ="连接到欧易 Web3 钱包"
    learn_more = "了解更多"
    blue_connect = "蓝牙连接说明"
    qr_connect = "二维码连接"
    start_setting = "开始设置"
    link_wallet = "连接钱包"
    download_app ="下载App"
    word_check = "单词 #{}"
    nearing_completion="即将完成"
    start_backup = "开始备份"
    enable_protection='启用保护'
    prepare_create='准备创建'
    prepare_recover='准备导入'
    prepare_check='准备核对'
    activate_device = "激活设备"
    enter_old_pin = "输入原 PIN"
    enter_new_pin = "输入新 PIN"
    enter_pin = "输入 PIN"
    enter_pin_again = "再次输入 PIN"
    select_language = "语言"
    create_wallet = "创建新钱包"
    wallet = "钱包"
    import_wallet = "导入钱包"
    restore_wallet = "恢复钱包"
    wallet_is_ready = "钱包已就绪"
    select_word_count = "选择单词数量"
    wallet_security = "钱包安全"
    pin_security = "PIN 安全提示"
    mnemonic_security = "助记词安全提示"
    backup_mnemonic = "助记词"
    enter_mnemonic = "输入助记词"
    check_mnemonic = "检查助记词"
    success = "成功"
    warning = "警告"
    error = "错误"
    verified = "助记词备份完成"
    invalid_mnemonic = "无效的助记词"
    terminate_mnemonic ="终止核对"
    pin_not_match = "不匹配"
    check_recovery_mnemonic = "检查恢复助记词"
    mnemonic_not_match = "助记词不匹配"
    power_off = "关机"
    restart = "重新启动"
    change_language = "更改语言"
    wipe_device = "抹掉设备"
    bluetooth_pairing = "蓝牙配对"
    address="{} 地址"
    public_key = "{} 公钥"
    xpub = "{} XPub"
    transaction = "{} 交易"
    transaction_detail = "交易详情"
    confirm_transaction = "确认交易"
    confirm_message = "确认消息"
    signature = "签名结果"
    wrong_pin = "PIN 错误"
    pin_changed = "PIN 已修改"
    pin_enabled = "PIN 已启用"
    pin_disabled = "PIN 已禁用"
    pin_removed = "PIN 已删除"
    unknown_token = "未知代币"
    view_data = "查看数据"
    sign_message = "{} 消息签名"
    verify_message = "{} 消息验签"
    typed_data = "{} 结构化数据"
    typed_hash = "{} 结构化哈希"
    system_update = "系统升级"
    entering_boardloader = "Entering Boardloader"
    remove_credential = "删除凭证"
    list_credentials = "列出凭证"
    authorize_coinjoin = "授权CoinJoin"
    multisig_address_m_of_n = "{} 多重签名地址\n({} of {})"
    u2f_register = "U2F 注册"
    u2f_unregister = "U2F 注销"
    u2f_authenticate = "U2F 认证"
    fido2_register = "FIDO2 注册"
    fido2_unregister = "FIDO2 注销"
    fido2_authenticate = "FIDO2 认证"
    fee_is_high = "手续费过高"
    confirm_locktime = "确认锁定时间"
    view_transaction = "查看交易"
    x_confirm_payment = "{} 确认付款"
    confirm_replacement = "确认替换交易"
    x_transaction = "{} 交易"
    x_joint_transaction = "{} 联合交易"
    change_label = "修改设备名称"
    enable_passphrase = "启用 Passphrase"
    disable_passphrase = "禁用 Passphrase"
    passphrase_source = "Passphrase 输入设置"
    enter_passphrase ="输入 Passphrase"
    enable_safety_checks ="启用安全检查"
    disable_safety_checks ="禁用安全检查"
    experiment_mode = "实验模式"
    set_as_homescreen = "设置为主屏幕"
    get_next_u2f_counter = "获取U2F计数器"
    set_u2f_counter = "设置U2F计数器"
    encrypt_value = "加密数据"
    decrypt_value = "解密数据"
    confirm_entropy = "导出熵"
    memo = "备注"
    import_credential = "导入凭证"
    export_credential = "导出凭证"
    asset = "资产"
    unimplemented = "未实现"
    invalid_data="无效的数据格式"
    low_power = "电量低"
    collect_nft = "收集NFT"
    reset_device ="重置此设备"
    device_protection_verification_failed = "验证未通过"
    verify_device ="验证设备"
    correct = "正确"
    spl_create_ata = "创建代币账户"
    select_derivation_path = "选择派生路径"

class Text:
    note = "注意"
    export_signed_transactions ="导出已签署的交易"
    export_signed_transactions_desc ="返回APP，并扫描下方的二维码"
    detail ="详情"
    transaction_send = "发送 {} {}"
    transaction_fee = "费用"
    transaction_direction = "转出"
    transaction_from = "发送方"
    transaction_to = "接收方"
    gas_limit = "交易费上限"
    gas_fee_cap = "单位Gas的交易费上限"
    gas_premium = "单位Gas的优先费用"
    wallet_not_match ="钱包不匹配"
    wallet_not_match_desc ="当前硬件钱包和app中选择的钱包不匹配，请检查是否Passphrase输入有误或钱包选择错误。"
    transaction_signed ="交易已签署"
    transaction_signed_desc ="您的交易已签署"
    invalid_transaction ="无效的交易"
    invalid_transaction_desc ="无效的交易数据，请重试"
    scan_app="扫描 App 上显示的二维码"
    use_passphrase ="使用此 Passphrase？"
    use_passphrase_desc ="请核对已输入的短语，确保其完全正确。"
    enter_mnemonic_desc="请按照顺序输入单词，确保编号与您的助记词备份完全匹配。"
    limit_error ="错误已达上限"
    limit_error_desc ="PIN 码错误次数已达上限，设备即将在 #28C600 5# 秒后重置"
    backup_completed="备份完成"
    backup_completed_desc="您已成功备份助记词"
    MetaCard_pro="Sealer MetaCard"
    passphrase_disable ="Passphrase已禁用，目前仅支持添加标准钱包"
    passphrase_disable_desc ="要禁用Passphrase（隐藏钱包）功能吗？"
    passphrase_enable ="Passphrase 功能已启用。每次连接 Sealer2100 应用时，设备均需输入 Passphrase，请牢记你设置的Passphrase"
    passphrase_enable_desc ="启用后，可在App端根据不同的Passphrase添加出不同的隐藏钱包"
    remove_pin_desc = "请确认是否要删除 PIN 码，仅保留虹膜识别？"
    enable_pin_ok ="你已成功启用 PIN 。"
    enable_pin_titles =[
        "高强度 PIN 码",
        "妥善保存"
    ]
    enable_pin_tips =[
        "请用高强度的 PIN 码，以防止钱包被未经授权的操作访问。",
        "妥善保存 PIN 码，并确保与助记词分开存放。"
    ]
    keyboard_feedback="键盘触感反馈"
    keyboard_feedback_desc ="与系统交互时提供振动反馈"
    enable_pin ="启用 PIN 码保护"
    set_pin_length ="设置一个长度为 #28C600 4# 到 #28C600 16# 位的 PIN 码，用于保护您的钱包。"
    reset_device_desc ="此操作将清空设备的内部存储以及安全芯片  (SE) 中的所有数据。在继续之前，请知晓以下事项："
    reset_device_check = [
        "重置后，此设备上你的助记词将被永久删除。",
        "您仍然可以通过助记词备份找回您的钱包和资产。"
    ]
    reset_warning = "警告\n恢复硬件设备至出厂设置，此操作将清除设备上的所有数据。"
    defi_disable ="DeFi 交易被禁止"
    defi_disable_desc ="DeFi锁开启期间，将禁止所有除转账以外的交易行为。"
    defi_lock_desc="DeFi 锁开启后，将禁止所有除转账以外的交易行为。"
    scan_learn_more_verify_device="扫码了解更多验证方法"

    verify_device_desc = [
        {
            "title": "下载 HPX Wallet",
            "message": "从以下地址下载 HPX Wallet App，移动端：",
        },
        {
            "title": "验证设备",
            "message": "打开HPX Wallet App 进入Web3 并连接您的设备以创建钱包。设备验证将自动进行。",
        }
    ]
    check_security_enable_desc ="启用安全检查后，将禁止派生不符合 BIP-39 协议的地址，并避免执行存在潜在安全风险或费用过高的交易。"
    scan_learn_more=  "扫码了解更多教程"
    system_update_desc=[
        {
            "title": "下载 HPX Wallet",
            "message": "从以下地址下载 HPX Wallet App，移动端：",
        },
        {
            "title": "开始更新",
            "message": "打开HPX Wallet App应用，前往“Web3钱包”，选择“导入钱包”>“连接硬件钱包”>“Sealer2100”，然后进入钱包管理，选择当前硬件后进入管理选项的“系统更新”。",
        }
    ]
    download_app = "前往下载 HPX 应用程序："
    serial_number = '序列号'
    model = '型号'
    bluetooth_name = "蓝牙名称"
    system_version = '系统版本'
    bluetooth_version = '蓝牙'
    bootloader="Bootloader"
    fcc="FCC"
    compatible="兼容 Trezor"
    system_update="系统更新"
    bluetooth_name = "蓝牙名称"
    set_as_homescreen_desc = "确定要将当前 NFT 设置成为主屏幕吗？"
    enable_pin="启用 PIN 码"
    enable_pin_desc="您当前暂未启用 PIN 码，是否激活 PIN  码，开启双重验证保护？"
    iris_operation_starting = "启动中..."
    iris_start_timeout = "虹膜启动超时"
    iris_regist = "虹膜注册"
    iris_match = "虹膜验证"
    iris_eye_focus = "将您的眼睛对准圆圈内"
    iris_removing_old = "正在移除旧虹膜"
    iris_changed="虹膜已更改"
    iris_changed_ok="您已成功更改虹膜。"
    iris_changed_failed="更改虹膜失败。"
    iris_removed = "虹膜已移除"
    iris_removed_ok = "您已成功移除虹膜。"
    iris_removed_failed = "移除虹膜失败。"
    front_of_screen ="请位于屏幕正前方"
    near_of_screen="请您适当靠近一些"
    far_of_screen="请您适当后退一些"
    iris_regist_suggestion_distance_close="后退一些，在20-30cm内注册"
    iris_regist_suggestion_distance_far="靠近一些，在20-30cm内注册"
    iris_match_suggestion_distance_close="后退一些，在20-30cm识别"
    iris_match_suggestion_distance_far="靠近一些，在20-30cm内识别"
    iris_suggestion_registing="正在注册，请勿移动"
    iris_suggestion_matching="正在识别，请勿移动"
    iris_operation_matching = "识别中..."
    iris_operation_registing = "注册中..."
    iris_operation_match_done = "识别成功"
    iris_operation_regist_done = "注册成功"
    iris_current_distance ="当前距离 "
    new_iris_registration="新虹膜注册"
    new_iris_registration_desc="开始录入新的虹膜，用于后续的安全验证。"
    change_iris="更改虹膜"
    remove_iris="移除虹膜"
    remove_iris_desc= "删除虹膜验证将显著降低您的设备安全等级，仅保留 PIN 码验证可能无法提供足够保护，建议谨慎操作。"
    iris_verification ="原虹膜验证"
    iris_verification_desc ="验证您设备上原有的虹膜信息"
    change_iris_desc="是否要删除当前已注册虹膜信息后重新录入新的虹膜？"
    register_iris_ok ="虹膜录入成功"
    register_iris_error ="虹膜录入失败"
    verify_iris_error ="虹膜验证失败"
    verify_iris_error_desc ="虹膜验证失败，请重试"
    register_iris_timed_out ="虹膜录入超时，请重试"

    iris_ok_desc ="已成功添加虹膜，可用于安全识别验证。"
    iris_desc="注册时请您摘下眼镜、墨镜或美瞳等佩戴物，保证裸眼完成注册。"
    iris_entry_desc ="因为此类物件在注册虹膜时可能因反光或遮挡导致注册虹膜的不完整，造成硬件后续无法解锁。"
    register_iris="注册虹膜"
    register_iris_desc="当前暂未启用更高安全性的虹膜验证，是否立即注册虹膜，开启双重验证保护？"
    suggest_enable_iris = "为提升安全性，建议同时录入虹膜信息，开启 PIN 码和虹膜双重保护。"
    iris_duplicate = "虹膜已注册"
    iris_duplicate_dsc = "当前虹膜已经注册，不需要进行更改"
    iris_note = "请再次确认你目前没有佩戴任何眼镜和美瞳等遮挡眼睛的物件。"
    random= "随机"
    default ="默认"
    pin_random="PIN 键盘上的数字随机排列。"
    pin_default="PIN 键盘上的数字按顺序排列。"
    pin_changed ="你已成功更改 PIN"
    airgap_mode_desc ="开启仅支持 Air Gap 后，将禁用设备通过蓝牙连接 App 功能。"
    airgap_mode = "仅支持 Air Gap"
    screen_wake_up_tap="轻点唤醒"
    screen_wake_up_button="按键唤醒"
    auto_shutdown_desc = "在设备熄屏状态下，若连续 {}无操作，系统将自动关机。"
    auto_shutdown_never_desc =  "在设备熄屏状态下，系统不会自动关机"
    screen_wake_up_tap_desc ="当前设置为屏幕锁定状态，轻触屏幕即可唤醒。"
    screen_wake_up_button_desc ="当前设置为屏幕锁定状态，按下电源键即可唤醒屏幕。"
    dynamic_effects_disabled_desc = "已禁用用户界面的动态效果。"
    dynamic_effects_allow_desc = "允许用户界面中的动态效果"
    vibration_desc ="与系统交互时提供振动反馈"
    auto_lock_desc = "在连续 {}无操作后，屏幕会自动熄灭并锁定。"
    auto_lock_never_desc = "不自動熄屏。"
    metamask_connect_desc ="ETH和EVM网络"

    blue_connect_hpx_wallet = [
        {
            "title": "下载 HPX Wallet",
            "message": "从以下地址下载 HPX Wallet App，移动端：",
        },
        {
            "title": "通过蓝牙连接",
            "message": [
                "• 打开手机蓝牙，并将硬件钱包靠近手机。",
                "• 确认硬件未开启「仅 AirGap 模式」，该模式在设备初始化时默认关闭。",
                "• 在App中选择搜索到的设备名称完成连接。",
                "#FFFFFF 提示：#若已手动开启「仅 AirGap 模式」，蓝牙功能将被禁用，需在硬件设置中关闭该模式后才能通过蓝牙连接App。",
            ]
        },
        {
            "title": "配对设备",
            "message": "在HPX Wallet应用中输入Sealer2100的配对码，验证后选择“开始”。"
        }
    ]

    blue_connect_okx_wallet = [
        {
            "title": "访问钱包",
            "message": '首次打开OKX移动应用，前往“Web3钱包”，选择“导入钱包”>“连接硬件钱包”>“Sealer2100”',
        },
        {
            "title": "通过蓝牙连接",
            "message": "打开两个设备的蓝牙。保持钱包靠近手机，点击监测到的Sealer2100设备名称。"
        },
        {
            "title": "导入钱包账户",
            "message": "完成配对后输入 PIN 码或虹膜识别，等待钱包列表加载后导入所需账户。"
        }
    ]

    hpx_wallet = "HPX 钱包"
    okx_wallet = "欧易 Web3 钱包"
    in_connection ="连接中..."
    in_connection_desc ="将 Sealer MetaCard 和设备保持在一起，知道传输完成。"
    mismatch ="不匹配"
    mismatch_desc ="您输入的Sealer MetaCard PIN 码不匹配，请重试。"
    need_help ="需要帮助？"
    need_help_desc ="若有问题需要解答，请前往 HPX 帮助中心寻求帮助。"
    swipe_up_to_apps ="向上滑动查看应用程序"
    processing = "处理中，请稍候..."
    signing = "正在构建交易..."
    loading= "正在加载交易..."
    connect_wallet_1 = "点击“连接硬件钱包”。"
    connect_wallet_2 = "连接设备“{}”。"
    connect_wallet_3 = "按引导提示完成操作后，您硬件中的钱包账户将恢复至HPX Web3钱包列表中。"
    word_error = "单词错误"
    word_error_desc = "单词输入有误，请核对您的备份后再尝试。"
    verification_passed = "验证通过"
    verification_passed_desc = "助记词验证已完成。"
    correct = "正确！"
    incorrect = "错误！"
    backup_title = [
        "妥善保管",
        "避免拍照",
        "保持安全"
    ]
    backup_check = [
        "助记词的遗失将导致您无法找回钱包内的所有资产，请务必妥善保管。",
        "助记词不得以任何方式拍照、生成数字副本或通过网络上传，以避免泄露风险。",
        "请将助记词记录并存放在安全的地方，切勿与他人分享或转发。"
     ]
    recommend = "推荐"
    please_wait = "请稍后..."
    my_wallet = "我的钱包"
    tap_to_unlock = "轻触屏幕以解锁"
    swipe_to_view_app = "向上滑动查看应用程序"
    unlocking = "正在解锁设备..."
    # str_words = "#18794E {}# 个单词"
    str_words = "{} 个单词"
    backup_manual = "手动写下助记词并存放在安全的地方"
    check_manual = "按顺序依次点击下面的单词"
    backup_verified = "您已完成助记词的备份，请妥善保存,不要与任何人分享"
    backup_invalid = "您输入的助记词不正确,请检查备份的助记词，再次尝试"
    pin_not_match = "两次的 PIN 码输入不一致，请重新输入。"
    pin_error = "输入的 PIN 不正确"
    wiping_device = "正在清除设备数据..."
    create_wallet = "生成一组新的助记词，创建新钱包"
    restore_wallet = "从您备份的助记词恢复钱包"
    restore_mnemonic_match = "您的助记词匹配，您的助记词备份正确"
    restore_success = "恢复钱包成功"
    create_success = "您的助记词已成功备份，钱包已创建"
    check_recovery_mnemonic = "请检查您的助记词，确认是否完全匹配"
    invalid_recovery_mnemonic = "输入的助记词无效，请核对您的备份后再尝试。"
    valid_recovery_mnemonic = "输入的助记词已完全匹配，您的备份无误。"
    check_recovery_not_match = "您输入的助记词是有效的，但是和设备中的助记词不匹配。"
    shutting_down = "正在关机..."
    restarting = "正在重新启动..."
    never = "从不"
    second = "秒"
    seconds = "秒"
    minute = "分钟"
    minutes = "分钟"
    changing_language = "您正在更改语言\n应用此设置将重新启动设备"
    change_pin = "设置一个长度为 #28c600 4# 至 #28c600 16# 位的 PIN 码，保护您的设备"
    wipe_device = "将设备恢复到出厂状态。\n警告： 这将从您的设备中抹掉所有数据。"
    wipe_device_check = [
        "抹掉设备会清除所有数据",
        "数据将无法恢复",
        "已经备份了助记词",
    ]
    wipe_device_success = "设备已成功清除数据\n 正在重启设备 ..."
    bluetooth_pair = "请在您的设备上输入配对码"
    bluetooth_pair_failed = "蓝牙配对失败"
    path = "派生路经:"
    chain_id = "Chain ID:"
    send = "发送"
    to = "至"
    amount = "金额"
    from_ = "发送者"
    receiver = "接收者"
    fee = "手续费"
    max_fee = "最大手续费"
    max_priority_fee_per_gas = "最大优先级手续费"
    max_fee_per_gas = "单位Gas的交易费上限"
    max_gas_limit = "最大Gas上限"
    gas_unit_price= "单位Gas价格"
    gas_price = "燃料价格"
    total = "总金额"
    do_sign_this_transaction = "是否签名这笔{}交易"
    transaction_signed = "交易已签名"
    address = '地址:'
    public_key = "公钥:"
    xpub = "XPub:"
    unknown_tx_type = "未知交易类型, 请检查输入数据"
    unknown_function = "未知的函数"
    use_app_scan_this_signature = "请使用钱包 APP 扫描签名结果"
    internal_error = "内部错误"
    tap_switch_to_airgap = "点击二维码切换为展示Airgap地址"
    tap_switch_to_receive = "点击二维码切换为展示钱包收款地址"
    incorrect_pin_times_left = "PIN 不正确, 剩余 {} 次重试机会"
    incorrect_pin_last_time = "PIN 不正确, 还有最后一次机会"
    wrong_pin = "输入的 PIN 码不正确"
    seedless = "缺少种子"
    backup_failed = "备份失败！"
    need_backup = "需要备份！"
    pin_not_set = "没有设置 PIN！"
    experimental_mode = "实验模式"
    pin_change_success = "PIN 码已成功更改。"
    pin_enable_success = "PIN 码已成功启用。"
    pin_disable_success = "PIN 码已成功禁用。"
    pin_remove_success = "PIN 码已成功移除。"
    contract = "合约:"
    new_contract = "新合约?"
    bytes_ = "{} 字节"
    message = "消息:"
    no_message = "不包含消息"
    contains_x_key = "包含 {} 键"
    array_of_x_type = "数组类型 {} {}"
    do_sign_712_typed_data = "是否签署这笔结构化数据交易？"
    do_sign_typed_hash = "是否签署这笔结构化哈希交易？"
    domain_hash = "Domain 哈希:"
    message_hash = "消息 哈希:"
    switch_to_update_mode = "切换到更新模式"
    switch_to_boardloader = "Switch to board loader mode"
    list_credentials = "是否要导出有关存储在此设备上的凭证信息？"
    coinjoin_at_x = "是否要参加下面的Coinjoin交易:\n{}"
    signature_is_valid = "签名有效"
    signature_is_invalid = "签名无效"
    u2f_already_registered = "U2F 已经注册"
    u2f_not_registered = "U2F 未注册"
    fido2_already_registered_x = "FIDO2 已注册 {}"
    fido2_verify_user = "FIDO2 验证用户"
    device_already_registered_x = "设备已注册 {}"
    device_verify_user = "设备验证用户"
    finalize_transaction = "完成交易"
    meld_transaction = "融合交易"
    update_transaction = "更新交易"
    fee_is_unexpectedly_high = "手续费过高"
    too_many_change_outputs= "太多的找零地址"
    change_count = "找零数量"
    locktime_will_have_no_effect = "锁定时间将不会有影响"
    confirm_locktime_for_this_transaction = "确认交易的锁定时间"
    block_height = "区块高度"
    time = "时间"
    amount_increased = "金额增加"
    amount_decreased = "金额减少"
    fee_unchanged = "手续费未改变"
    fee_increased = "手续费增加"
    fee_decreased = "手续费减少"
    your_spend = "你的花费"
    change_label_to_x = "修改标签为 {}"
    enable_passphrase = "要启用 Passphrase 加密吗？"
    disable_passphrase = "要禁用 Passphrase 加密吗？"
    enable_passphrase_always = "总是在本机输入 Passphrase 吗？"
    revoke_enable_passphrase_always = "要撤销总是在本机输入 Passphrase 的设置吗？"
    auto_lock_x = "确定要在 {} 之后自动锁定设备吗？"
    enable_safety_checks = "要执行严格的安全检查吗？这将提供更全面的安全保护。"
    disable_safety_checks = "确定要禁用安全检查吗？继续操作前，请知悉该行为的潜在安全风险。"
    enable_experiment_mode = "是否启用实验模式？"
    set_as_homescreen = "确定要更改主屏幕吗？"
    replace_homescreen = "确定要替换主屏幕吗？这将会删除最早上传的壁纸。"
    confirm_replace_wallpaper = "确定要替换主屏幕吗的壁纸吗？"
    get_next_u2f_counter = "确定要获取下一个U2F计数器吗？"
    set_u2f_counter_x = "确定要设置U2F计数器为 {} 吗？"
    confirm_entropy = "确定要导出熵吗？继续操作之前请明白你在做什么！"
    bandwidth = "带宽"
    energy = "能量"
    sender = "发送者"
    recipient = "接收者"
    resource = "资源"
    frozen_balance = "冻结的金额"
    unfrozen_balance = "解冻的金额"
    delegated_balance = "委托的金额"
    undelegated_balance = "撤销委托的金额"
    you_are_freezing = "你正在冻结资产"
    you_are_unfreezing = "你正在解冻资产"
    you_are_delegating = "你正在委托资产"
    you_are_undelegating = "你正在撤销委托"
    duration = "持续时间"
    lock = "锁定"
    unlock = "解锁"
    all = "全部"
    source = "来源"
    tip = "提示"
    keep_alive = "Keep alive"
    invalid_ur = "不支持的二维码类型，请重试"
    sequence_number = "序列号"
    expiration_time = "过期时间"
    argument_x = "参数 #{}"
    low_power_message = "电量还剩余 {}%\n请充电"
    collect_nft = "确定要收集此NFT吗？"
    replace_nft = "确定要收藏此 NFT 吗？由于数量已达上限，这将会移除最早上传的 NFT。"
    verify_device = "你确定要使用Sealer2100服务器验证你的设备吗？点击确认以检查你的设备是否为原装且未被篡改。"
    device_wakeup_by_touch_screen = "轻点唤醒"
    device_wakeup_by_click_power_button = "按键唤醒"
    on = "开"
    off = "关"
    input_passphrase_on_device_desc = "在需要输入Passphrase，在设备上输入"
    input_passphrase_on_host_desc = "在需要输入Passphrase，在应用程序上输入"
    already_input = "已提交"
    please_backup = "请立即备份您的新钱包。"

    ## iris error code
    iris_code_0 = "成功"
    iris_code_1 = "活体检测失败"
    iris_code_2 = "模组拒绝该命令"
    iris_code_3 = "超时失败"
    iris_code_4 = "虹膜已注册"
    iris_code_5 = "用户 id 已存在"
    iris_code_6 = "没有录入用户，模组中没有用户"
    iris_code_7 = "用户 id 不存在"
    iris_code_8 = "模组中用户数量已达最大值"
    iris_code_11 = "终止注册 / 识别过程"
    iris_code_12 = "参数错误"
    iris_code_13 = "相机打开失败"
    iris_code_14 = "模组初始化失败"
    iris_code_15 = "活体检测失败"

    ## iris notes message
    iris_note_module_status_1 = "模组启动成功"
    iris_note_module_status_2 = "模组启动识别"
    iris_note_module_status_3 = "模组启动注册"

    iris_note_regist_progress = "注册进度：{} \t {}"

    iris_note_iris_status_0 = "虹膜正常"
    iris_note_iris_status_1 = "距离太近"
    iris_note_iris_status_2 = "距离太远"
    iris_note_iris_status_3 = "未检测到虹膜"
    device_protection_verification_failed_msg = "PIN 码或虹膜不匹配，请重试。"
    device_protection_verification_left = "剩余 {} 次重试机会"
    power_off_confirm = "确定要关机吗？"
    too_many_iris_match_count = "虹膜匹配超时次数太多了\n\n是否要抹掉设备？"
    evm_address_desc="该地址为 EVM 通用地址，可同步管理其他EVM兼容链（如Ethereum、BNB Chain、Avalanche、Arbitrum、Optimism、Polygon等）上的资产。"

class Subtitle:
    account_qr_address ="返回 App 并扫描下方的二维码"
    confirm_transaction = "确认要签署该笔交易吗？"
    terminate_mnemonic ="终止本流程会导致所有的进度都将丢失，请确认是否要终止？"
    backup="您可以从以下备份方式中进行选择。一旦助记词通过验证，系统会将其作为备份内容保存。"
    enter_mnemonic = "请依照备份中的编号逐项输入助记词单词，确保完全一致。"
    airgap_mode_close="要关闭仅支持Air Gap模式吗？关闭后，将允许App通过蓝牙连接此设备。"
    airgap_mode_open="是否确认开启仅支持 Air Gap 模式？启用后，所有连接软件钱包及签署交易等操作都将通过扫描二维码在离线环境中完成。"
    qr_connect_wallet_desc = "打开 {} 扫描下方二维码，支持Ethereum 和 EVM 网络。"
    equipment_certification ="是否确定使用 HPX 安全服务器对您的设备进行防伪验证？点击确认键继续，以确保您的设备为正品且未被篡改。"
    learn_more = "扫描二维码，查看详细教程"
    choose_connection_wallet ="选择您想要连接的钱包。"
    choose_connection_method = "选择连接方式。"
    start_setting = "将 Sealer MetaCard 贴合设备的背面，然后点击继续。"
    import_wallet = "选择您想要的导入方式"
    nft_count = "{} 个展品"
    word_check = "请选择正确的单词"
    nearing_completion = "请按照提示，依次核对您备份的助记词单词。"
    backup_mnemonic = "按顺序抄写下方的 #00FE33 {}# 个单词。"
    start_backup ="接下来将展示一组称为助记词的单词。在此之前，请务必注意以下事项："
    wallet_is_ready = "新钱包创建成功！"
    enable_protection = "PIN 或虹膜识别是您的数字资产保险锁。开启后，所有敏感操作都将受到全面保护。"
    select_mnemonics = "选择助记词位数。"
    activate_device = "创建新的助记词，或导入已有助记词来恢复钱包。"
    create_wallet ="即将创建由一组新助记词生成的标准钱包。"
    reset_wallet = "将设备还原为出厂设置。"

class Tip:
    swipe_down_to_close = "向下滑动以关闭"

class Button:
    back ="返回"
    signature_transaction ="签名交易"
    enable ="启用"
    disable = "禁用"
    delete ="删除"
    reset = "重置"
    set_home_screen="设为主屏幕"
    iris_entry="录入虹膜"
    iris_not_now = "暂不录入"
    to_open="去开启"
    verify_now = "立即验证"
    learn_more ="了解更多"
    mnemonic= "助记词"
    metacard = "Sealer MetaCard"
    close = "关闭"
    enable_pin_iris = "同时启用 PIN 码和虹膜验证"
    enable_pin = "启用 PIN 码验证"
    enable_iris = "启用虹膜验证"
    create_wallet ="创建新钱包"
    import_wallet = "导入钱包"
    done = "完成"
    ok = "好的"
    confirm = "确定"
    reject = "拒绝"
    next = "下一步"
    cancel = "取消"
    redo = "重新生成"
    continue_ = "继续"
    try_again = "重试"
    power_off = "关机"
    restart = "重新启动"
    hold = "按住"
    address = "地址"
    qr_code = "二维码"
    view_detail = "查看详情"
    hold_to_sign = "长按签名"
    hold_to_wipe = "长按抹掉"
    receive = "收款地址"
    airgap = "Airgap"
    sign = "签名"
    verify = "验证"
    view_full_array = "查看完整数组"
    view_full_struct = "查看完整结构"
    view_full_message = "查看完整消息"
    view_data = "查看数据"
    view_more = "查看更多"
    start = "开始"
class WalletSecurity:
    header = "在一张纸上写下您的助记词并将其存放在安全的地方"
    tips = [
        {
            "level": "info",
            "msgs": [
                "#00001F 助记词需要安全存放#",
                "#18794E * 存放在银行金库中#",
                "#18794E * 存放在保险箱中#",
                "#18794E * 存放在多个秘密地点#",
            ]
        },
        {
            "level": "warning",
            "msgs": [
                "#00001F 一定要注意#",
                "#CD2B31 * 谨记保存的助记词#",
                "#CD2B31 * 不要丢失#",
                "#CD2B31 * 不要告诉别人#",
                "#CD2B31 * 不要存储在网络上#",
                "#CD2B31 * 不要存储在计算机上#",
            ]
        },
    ]

class MnemonicSecurity:
    header = "助记词是一组用来恢复钱包资产的短语，拥有助记词就意味着可以使用您的资产，请妥善保管"

    tips = [
        "1. 请检查环境安全，确保没有旁观者或摄像头",
        "2. 请按短语的正确顺序备份助记词，切勿与任何人分享您的助记词",
        "3. 请在安全的地方离线保存助记词，切勿使用电子方式备份助记词，切勿上传网络",
    ]

class PinSecurity:
    header = "PIN 码是设备访问的密码,用于授权访问当前设备。请按照以下提示正确使用PIN码"
    tips = [
        "1. 设置或录入 PIN 时请检查环境安全，确保没有旁观者或摄像头",
        "2. 请设置长度为 4-16 位的高强度的IN码，避免使用连续或重复的数字",
        "3. PIN 码的最大重试次数为10次，当错误10次后设备将会被重置",
        "4. 请妥善保管 PIN 码，不要和任何人分享您的 PIN 码",
    ]

class Solana:
    ata_reciver = "接收者(关联代币账户)"
    ata_sender = "发送者(关联代币账户)"
    source_owner = "交易签名者"
    fee_payer = "手续费支付者"
    associated_token_account = "新的代币账户"
    owner = "拥有者"
    mint_address = "铸币地址"
    found_by = "资助方"
    blinding_sign = "Solana 原始消息签名"
    blinding_sign_dsc = "允许签署原始 Solana 消息而不进行处理或验证。这可能会让您面临网络钓鱼、盲签和未经授权的批准。请谨慎使用。"

class Filecoin:
    gas_limit = "燃料用量上限"
    gas_fee_cap = "燃料单价上限"
    gas_premium = "优先费用"

class Ripple:
    destination_tag = "目的地标签"

