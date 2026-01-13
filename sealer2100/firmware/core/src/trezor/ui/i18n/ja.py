class App:
    account = "アカウント"
    scan = "スキャン取引"
    nft = "NFT ギャラリー"
    guide = "使用説明"
    security = "セキュリティ"
    setting = "設定"

#### Setting App
class Setting:
    bluetooth = "ブルートゥース"
    language = "言語"
    vibration = "触覚フィードバック"
    brightness = "画面の明るさ"
    auto_lock = "自動ロック"
    auto_shutdown = "自動シャットダウン"
    animation = "トランジションアニメーション"
    wallpaper = "壁紙"
    power_off = "電源オフ"
    restart = "再起動"

#### Security App
class Security:
    change_pin = "PINコードの変更"
    backup_mnemonic = "シードフレーズのバックアップ"
    check_mnemonic = "シードフレーズの確認"
    wipe_device = "デバイスの消去"

#### guide App
class Guide:
    about = "Digital Shieldについて"
    terms_of_use = '利用規約'
    device_info = 'デバイス情報'
    firmware_update = 'ファームウェア更新'
    terms_title_terms_us = 'Digit Shield利用規約'
    terms_describe_terms_us = '利用規約の完全版にアクセスするには、以下のリンクを参照してください:\n http://hypermate.com/terms'

    terms_title_product_services = 'Digit Shield製品とサービス'
    terms_describe_product_services = '当社のハードウェアウォレットは暗号資産を安全に管理します'
    terms_title_risks = 'リスク'
    terms_describe_risks = '暗号資産と技術的脆弱性に関連するリスクにご注意ください。'
    terms_title_disclaimers = '免責事項'
    terms_describe_disclaimers = '提供される情報は財務アドバイスではありません。いかなる決定を行う前にも専門家の助言を求めてください。'
    terms_title_contact_us = 'お問い合わせ'
    terms_describe_contact_us = 'ご質問や懸念がある場合は、www.ds.pro@gmail.comまでメールでお問い合わせください。'

    accept_tems = '一、条項の承諾'
    use_range = '1. 適用範囲'
    range_include = 'これらの条項は、Digital Shieldウォレットを通じて提供されるすべてのサービスに適用されます。'
    range_include_1 = 'ハードウェアウォレットの購入、アクティベーション、アフターサービス；Digital Shieldモバイルアプリケーション（Android/iOS/Goolg）のダウンロード、インストール、機能の使用；ファームウェアアップグレードサービス（セキュリティパッチ、機能強化バージョンを含む）；マルチチェーンデジタル資産管理（BTC、ETHなど3000以上のトークンの保存と転送をサポート）；技術サポート（デバイスの故障トラブルシューティング、トランザクション署名異常処理など）。'
    user_qualification = '2. ユーザー資格'
    ability_include = 'あなたは、18歳以上であり、完全な民事行為能力を有することを確認します。'
    ability_include_1 = 'お住まいの司法管轄区域では、暗号通貨および関連ハードウェアデバイスの使用が禁止されていません（例：中国本土のユーザーは、使用リスクを自己負担する必要があります）。'
    terms_update_infor = '3. 条項の更新と通知'
    update_infor_content = '私たちは、条項を一方的に変更する権利を留保します。改正内容は、公式ウェブサイトの公告欄で発表され、効力発生日は公告に準じます。サービスを継続して使用する場合、改正後の条項に同意したものとみなされます。同意しない場合、効力発生日までに書面で通知し、使用を中止してください。'

    wallet_buy_iterms = '二、 ハードウェアウォレットの購入条項'
    order_process = '1. 注文プロセス'
    payment_confirmation = '注文の支払いが成功した後（ブロックチェーンネットワークによる確認または銀行口座の入金を基準）、システムは24時間以内に状態を更新します。'
    inventory_shortage = '在庫不足：在庫が不足している場合、ユーザーは選択できます。'
    inventory_shortage_1 = 'a. 在庫補充を待つ（最大30日、期限を過ぎると自動的に返金されます）。'
    inventory_shortage_2 = 'b. 元の方法で完全に返金（暗号通貨注文は、支払い時の為替レートで計算されます）。'
    return_and_exchange_policy = '2. 返品・交換ポリシー'
    return_and_exchange_condi = '返品条件：'
    return_and_exchange_condi_con = 'a. 未アクティベートデバイスは、元の工場シールラベル（注文番号と一致）と完全なアクセサリー（USBケーブル、マニュアル、ニモニックカード）を保持する必要があります。b. 返品申請は、受領後7日以内に提出する必要があり、期限を過ぎると品質検査合格とみなされます。c. 返送送料は、ユーザーが負担します（品質問題を除く）。'
    warranty_scope = '保証範囲：'
    warranty_scope_1 = 'a. 安全チップの故障、ディスプレイの異常、ボタンの故障などの人的でない損傷をカバーします。'
    warranty_scope_2 = 'b. 購入証明（注文番号）と故障証明（動画はデバイスのSNコードと異常現象を明確に表示する必要があります）を提供する必要があります。'
    warranty_scope_3 = 'c. 人的損傷（水没、落下など）は保証範囲外であり、有償修理が可能です。'

    disclaimer = '三、免責事項'
    product_risk = '1. プロダクトリスク'
    physical_risk = '物理的リスク：'
    physical_risk_1 = 'a. デバイスは、高温（>60℃）、高湿度（>90% RH）、強磁場（>100mT）の環境では機能しない可能性があります。'
    physical_risk_2 = 'b. 長期間充電されていない場合、バッテリーが損傷する可能性があります（毎月1回充電することをお勧めします）。'
    supply_chain_risk = 'サプライチェーンリスク：'
    supply_chain_risk_1 = 'a. 公式ウェブサイトでは、偽造防止検証ツールを提供しており、デバイスのQRコードをスキャンして本物かどうかを確認できます。'
    supply_chain_risk_2 = 'b. デバイスが交換されたと疑われる場合、直ちにカスタマーサービスに連絡し、警察に報告してください。'
    service_interruption = '2. サービス中断'
    service_interruption_1 = '計画されたメンテナンスは、公式ウェブサイトの公告で48時間前に通知されます。緊急メンテナンスの場合、予告なしでサービスが一時停止される可能性があります。'
    service_interruption_2 = '不可抗力によるデータ損失については、賠償責任を負いません。'


    device_label = "デバイス名"
    device_title_firmware_version = 'ファームウェアバージョン'
    device_title_serial_number = 'シリアル番号'
    bluetooth_name = "Bluetooth名"
    bluetooth_version = "Bluetoothバージョン"
    firmware_title_1 = '1. デバイスのバッテリーが20%以上あることを確認してください'
    firmware_title_2 = '2. USB-CケーブルでデバイスをPCに接続してください'
    firmware_title_3 = '3. 「ファームウェア更新」をクリックしてください'
    firmware_title_caution = '警告'
    firmware_describe_caution = '更新中はUSB接続を維持してください'
    equipment_info = 'デバイス情報'
    equipment_name = 'デバイス名'
    equipment_version = 'デバイスバージョン'

class Nft:
    nft_item ="{} アイテム"
    nft_items ="{} アイテム"
class Title:
    enter_old_pin = "旧PINコードを入力してください"
    enter_new_pin = "新しいPINコードを入力してください"
    enter_pin = "PINコードを入力してください"
    enter_pin_again = "PINコードを再入力してください"
    select_language = "言語"
    create_wallet = "ウォレットを作成"
    wallet = "ウォレット"
    import_wallet = "ウォレットをインポート"
    restore_wallet = "ウォレットを復元"
    wallet_is_ready = "ウォレットの準備が完了しました"
    select_word_count = "単語数を選択"
    wallet_security = "ウォレットのセキュリティ"
    pin_security = "PINセキュリティの注意事項"
    mnemonic_security = "シードフレーズの注意事項"
    backup_mnemonic = "シードフレーズをバックアップ"
    enter_mnemonic = "シードフレーズを入力してください"
    check_mnemonic = "シードフレーズを確認"
    success = "成功"
    operate_success = "操作が成功しました"
    theme_success = "主題切換成功"
    warning = "警告"
    error = "エラー"
    verified = "シードフレーズのバックアップ完了"
    invalid_mnemonic = "無効なシードフレーズ"
    pin_not_match = "PINが一致しません"
    check_recovery_mnemonic = "復元シードフレーズを確認"
    mnemonic_not_match = "シードフレーズが一致しません"
    power_off = "電源オフ"
    restart = "再起動"
    change_language = "言語を変更"
    wipe_device = "デバイスを消去"
    bluetooth_pairing = "Bluetoothペアリング"
    address="{} アドレス"
    public_key = "{} 公開鍵"
    xpub = "{} XPub"
    transaction = "{} トランザクション"
    transaction_detail = "取引詳細"
    confirm_transaction = "取引を確認"
    confirm_message = "メッセージを確認"
    signature = "署名結果"
    wrong_pin = "PINエラー"
    pin_changed = "PINが変更されました"
    pin_enabled = "PINが有効化されました"
    pin_disabled = "PINが無効化されました"
    unknown_token = "不明なトークン"
    view_data = "データを表示"
    sign_message = "{} メッセージ署名"
    verify_message = "{} メッセージ検証"
    typed_data = "{} 構造化データ"
    typed_hash = "{} 構造化ハッシュ"
    system_update = "システムアップデート"
    entering_boardloader = "ボードローダーに入る"
    remove_credential = "認証情報を削除"
    list_credentials = "認証情報を一覧表示"
    authorize_coinjoin = "CoinJoinを承認"
    multisig_address_m_of_n = "{} マルチシグアドレス\n({} of {})"
    u2f_register = "U2F登録"
    u2f_unregister = "U2F登録解除"
    u2f_authenticate = "U2F認証"
    fido2_register = "FIDO2登録"
    fido2_unregister = "FIDO2登録解除"
    fido2_authenticate = "FIDO2認証"
    finalize_transaction = "取引を確定"
    meld_transaction = "取引を統合"
    update_transaction = "取引を更新"
    high_fee = "高い料金"
    fee_is_high = "手数料が高すぎます"
    confirm_locktime = "ロックタイムを確認"
    view_transaction = "取引を表示"
    x_confirm_payment = "{} 支払いを確認"
    confirm_replacement = "取引の置換を確認"
    x_transaction = "{} 取引"
    x_joint_transaction = "{} 共同取引"
    change_label = "デバイス名を変更"
    enable_passphrase = "パスフレーズを有効化"
    disable_passphrase = "パスフレーズを無効化"
    passphrase_source = "パスフレーズ入力設定"
    enable_safety_checks ="セキュリティチェックを有効化"
    disable_safety_checks ="セキュリティチェックを無効化"
    experiment_mode = "実験モード"
    set_as_homescreen = "ホームスクリーンに設定"
    get_next_u2f_counter = "U2Fカウンターを取得"
    set_u2f_counter = "U2Fカウンターを設定"
    encrypt_value = "データを暗号化"
    decrypt_value = "データを復号化"
    confirm_entropy = "エントロピーをエクスポート"
    memo = "メモ"
    import_credential = "認証情報をインポート"
    export_credential = "認証情報をエクスポート"
    asset = "資産"
    unimplemented = "未実装"
    invalid_data="無効なデータ形式"
    low_power = "バッテリー残量が少ない"
    collect_nft = "NFT を収集する"
    verify_device = "デバイスを検証する"
    update_bootloader = "ブートローダーを更新する"
    update_resource = "リソースを更新する"
    need_input_tips = "すべてのメモニックワードを入力してから、次のステップをクリックしてください。"
    need_select_tips = "メモニックワードの順番でクリックして選択し、完了したら次のステップをクリックしてください。"
    return_check_mnemonic = "メモニックワードを確認する"

class Text:
    tap_to_unlock = "タップしてロック解除"
    unlocking = "デバイスのロック解除中..."
    str_words = "#FFFFFF {}# 単語"
    backup_manual = "シードフレーズを手書きで安全な場所に保管してください"
    check_manual = "以下の単語を順番にタップしてください"
    backup_verified = "シードフレーズのバックアップが完了しました。安全に保管し、誰とも共有しないでください"
    backup_invalid = "入力したシードフレーズが正しくありません。バックアップを確認して再度お試しください"
    pin_not_match = "入力したPINコードが正しくありません。再度お試しください"
    please_wait = "お待ちください"
    wiping_device = "デバイスデータを消去中..."
    create_wallet = "新しいシードフレーズを生成して新規ウォレットを作成"
    restore_wallet = "バックアップしたシードフレーズからウォレットを復元"
    restore_mnemonic_match = "シードフレーズが一致しました。バックアップは正しく行われています"
    restore_success = "ウォレットの復元に成功しました"
    create_success = "シードフレーズのバックアップが成功し、ウォレットが作成されました"
    check_recovery_mnemonic = "シードフレーズを確認し、完全に一致しているか確認してください"
    invalid_recovery_mnemonic = "入力したシードフレーズは無効です。シードフレーズを確認して再度お試しください"
    check_recovery_not_match = "入力したシードフレーズは有効ですが、デバイス内のシードフレーズと一致しません"
    shutting_down = "シャットダウン中..."
    restarting = "再起動中..."
    never = "なし"
    second = "秒"
    seconds = "秒"
    minute = "分"
    minutes = "分"
    changing_language = "言語を変更しています\nこの設定を適用するとデバイスが再起動します"
    change_pin = "4～16桁のPINコードを設定してデバイスを保護してください"
    wipe_device = "デバイスを工場出荷時の状態にリセットします\n警告: デバイス内のすべてのデータが消去されます"
    wipe_device_check = [
        "デバイスを消去するとすべてのデータが削除されます",
        "データは復元できません",
        "シードフレーズをバックアップ済みです",
    ]
    wipe_device_success = "デバイスのデータ消去が完了しました\nデバイスを再起動しています..."
    bluetooth_pair = "デバイスにペアリングコードを入力してください"
    bluetooth_pair_failed = "Bluetoothペアリングに失敗しました"
    path = "派生パス:"
    chain_id = "チェーンID:"
    send = "送金"
    to = "宛先"
    amount = "金額"
    from_ = "送金元"
    receiver = "受取人"
    fee = "手数料"
    max_fee = "最大手数料"
    max_priority_fee_per_gas = "最大優先手数料"
    max_fee_per_gas = "Gasあたりの最大手数料"
    max_gas_limit = "最大ガス制限:"
    gas_unit_price = "ガス単価:"
    gas_price = "Gas価格"
    total = "合計金額"
    do_sign_this_transaction = "この{}取引に署名しますか？"
    transaction_signed = "取引の署名が完了しました"
    address = 'アドレス:'
    public_key = "公開鍵:"
    xpub = "XPub:"
    unknown_tx_type = "不明な取引タイプです。入力データを確認してください"
    unknown_function = "未知の関数:"
    use_app_scan_this_signature = "ウォレットアプリで署名結果をスキャンしてください"
    internal_error = "内部エラー"
    tap_switch_to_airgap = "QRコードをタップしてAirgapアドレス表示に切り替え"
    tap_switch_to_receive = "QRコードをタップして受取アドレス表示に切り替え"
    incorrect_pin_times_left = "PINが不正です。あと{}回試行できます"
    incorrect_pin_last_time = "PINが不正です。最後の試行です"
    wrong_pin = "入力されたPINコードが正しくありません"
    seedless = "シードがありません"
    backup_failed = "バックアップ失敗！"
    need_backup = "バックアップが必要です！"
    pin_not_set = "PINが設定されていません！"
    experimental_mode = "実験モード"
    pin_change_success = "PINコードの変更に成功しました"
    pin_enable_success = "PINコードの有効化に成功しました"
    pin_disable_success = "PINコードの無効化に成功しました"
    contract = "コントラクト:"
    new_contract = "新しいコントラクト？"
    bytes_ = "{} バイト"
    message = "メッセージ:"
    no_message = "メッセージなし"
    contains_x_key = "{} キーを含む"
    array_of_x_type = "{} {} の配列"
    do_sign_712_typed_data = "この構造化データ取引に署名しますか？"
    do_sign_typed_hash = "この構造化ハッシュ取引に署名しますか？"
    domain_hash = "ドメインハッシュ:"
    message_hash = "メッセージハッシュ:"
    switch_to_update_mode = "更新モードに切り替え"
    switch_to_boardloader = "ボードローダーモードに切り替え"
    list_credentials = "このデバイスに保存されている認証情報をエクスポートしますか？"
    coinjoin_at_x = "以下のCoinjoin取引に参加しますか？:\n{}"
    signature_is_valid = "署名は有効です"
    signature_is_invalid = "署名は無効です"
    u2f_already_registered = "U2Fは既に登録済みです"
    u2f_not_registered = "U2Fは未登録です"
    fido2_already_registered_x = "FIDO2は既に登録済み {}"
    fido2_verify_user = "FIDO2 ユーザー認証"
    device_already_registered_x = "デバイスは既に登録済み {}"
    device_verify_user = "デバイスによるユーザー認証"
    fee_is_unexpectedly_high = "手数料が異常に高い"
    too_many_change_outputs = "釣り銭の出力が多すぎます"
    change_count = "お釣りの数"
    locktime_will_have_no_effect = "ロックタイムは無効になります"
    confirm_locktime_for_this_transaction = "この取引のロックタイムを確認"
    block_height = "ブロック高"
    time = "時間"
    amount_increased = "金額が増加"
    amount_decreased = "金額が減少"
    fee_unchanged = "手数料は変更なし"
    fee_increased = "手数料が増加"
    fee_decreased = "手数料が減少"
    your_spend = "あなたの支出"
    change_label_to_x = "ラベルを {} に変更"
    enable_passphrase = "Passphrase暗号化を有効にしますか？"
    disable_passphrase = "Passphrase暗号化を無効にしますか？"
    enable_passphrase_always = "常にローカルでPassphraseを入力しますか？"
    revoke_enable_passphrase_always = "常にローカルでPassphraseを入力する設定を解除しますか？"
    auto_lock_x = "{}後にデバイスを自動ロックしますか？"
    enable_safety_checks = "厳格なセキュリティチェックを実行しますか？より包括的な保護が提供されます"
    disable_safety_checks = "セキュリティチェックを無効にしますか？続行前に潜在的なリスクを理解してください"
    enable_experiment_mode = "実験モードを有効にしますか？"
    set_as_homescreen = "ホームスクリーンを変更しますか？"
    replace_homescreen = "ホームスクリーンを置き換えますか？最初にアップロードした壁紙が削除されます"
    confirm_replace_wallpaper = "ホームスクリーンの壁紙を置き換えますか？"
    get_next_u2f_counter = "次のU2Fカウンターを取得しますか？"
    set_u2f_counter_x = "U2Fカウンターを {} に設定しますか？"
    confirm_entropy = "エントロピーをエクスポートしますか？続行前にその意味を理解してください！"
    bandwidth = "帯域幅"
    energy = "エネルギー"
    sender = "送信者"
    recipient = "受信者"
    resource = "リソース"
    frozen_balance = "凍結残高"
    unfrozen_balance = "解凍残高"
    delegated_balance = "委任残高"
    undelegated_balance = "委任解除残高"
    you_are_freezing = "資産を凍結中です"
    you_are_unfreezing = "資産を解凍中です"
    you_are_delegating = "資産を委任中です"
    you_are_undelegating = "委任を解除中です"
    duration = "期間"
    lock = "ロック"
    unlock = "アンロック"
    all = "すべて"
    source = "ソース"
    tip = "ヒント"
    keep_alive = "キープアライブ"
    invalid_ur = "サポートされていないQRコード形式です。再試行してください"
    sequence_number = "シーケンス番号"
    expiration_time = "有効期限"
    argument_x = "引数 #{}"
    low_power_message = "バッテリー残量 {}%\n充電してください"
    collect_nft = "この NFT を収集することを確認しますか？"
    replace_nft = "この NFT を収集しますか？ストレージの上限に達しており、最も古いアップロードされた NFT が削除されます。"
    verify_device = "HyperMateサーバーでデバイスを認証してもよろしいですか？ 確認をタップして、デバイスが本物で改竄されていないことを確認してください。"
    update_bootloader = "ブートローダーを更新しますか？"
    update_resource = "デバイスのリソースを更新しますか？"

class Tip:
    swipe_down_to_close = "下にスワイプして閉じる"

class Button:
    done = "完了"
    ok = "OK"
    confirm = "確認"
    reject = "拒否"
    next = "次へ"
    cancel = "キャンセル"
    redo = "再生成"
    continue_ = "続ける"
    try_again = "再試行"
    power_off = "電源オフ"
    restart = "再起動"
    hold = "長押し"
    address = "アドレス"
    qr_code = "QRコード"
    view_detail = "詳細を表示"
    hold_to_sign = "長押しで署名"
    hold_to_wipe = "長押しで消去"
    receive = "受取アドレス"
    airgap = "Airgap"
    sign = "署名"
    verify = "検証"
    view_full_array = "配列全体を表示"
    view_full_struct = "構造全体を表示"
    view_full_message = "メッセージ全体を表示"
    view_data = "データを表示"
    view_more = "さらに表示"

class WalletSecurity:
    header = "紙にシードフレーズを書き留め、安全な場所に保管してください"
    tips = [
        {
            "level": "info",
            "msgs": [
                "#00001F シードフレーズは安全に保管する必要があります#",
                "#18794E * 銀行の金庫に保管#",
                "#18794E * セーフティボックスに保管#",
                "#18794E * 複数の秘密の場所に分散保管#",
            ]
        },
        {
            "level": "warning",
            "msgs": [
                "#00001F 重要な注意事項#",
                "#CD2B31 * シードフレーズの保管場所を忘れないでください#",
                "#CD2B31 * 紛失しないように注意#",
                "#CD2B31 * 他人に教えないでください#",
                "#CD2B31 * ネット上に保存しないでください#",
                "#CD2B31 * コンピュータに保存しないでください#",
            ]
        },
    ]

class MnemonicSecurity:
    header = "シードフレーズはウォレット資産を復元するための単語群です。シードフレーズを所有することは資産を操作できることを意味します。厳重に保管してください"

    tips = [
        "1. 周囲の安全を確認し、第三者やカメラがない環境で操作してください",
        "2. 正しい順序でシードフレーズをバックアップし、決して他人と共有しないでください",
        "3. 安全な場所でオフライン状態で保管し、電子機器やネットワーク上に保存しないでください",
    ]

class PinSecurity:
    header = "PINコードはデバイスアクセスのパスワードで、現在のデバイスへのアクセスを承認するために使用されます。以下の注意事項に従って正しく使用してください"
    tips = [
        "1. PINの設定や入力時は、周囲の安全を確認し、第三者やカメラがない環境で操作してください",
        "2. 4-16桁の強度の高いPINコードを設定し、連続した数字や繰り返し数字の使用は避けてください",
        "3. PINコードの最大試行回数は10回で、10回間違えるとデバイスはリセットされます",
        "4. PINコードを厳重に保管し、決して他人と共有しないでください",
    ]

class Solana:
    ata_reciver = "受信者(関連トークンアカウント)"
    ata_sender = "送信者(関連トークンアカウント)"
    source_owner = "トランザクション署名者"
    fee_payer = "手数料支払者"
