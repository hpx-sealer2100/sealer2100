class App:
    account = "Account"
    connect_app = "Connect App Wallet"
    backup = "Backup mnemonics"
    scan = "Scan"
    nft = "NFT Gallery"
    guide = "User Guide"
    security = "Security"
    setting = "Settings"

# Setting App
class Setting:
    check_security = "Security Check"
    check_mnemonic ="Check mnemonic"
    remove_pin = "Remove PIN"
    change_pin = "Change PIN"
    enable_pin = "Enable PIN"
    remove_iris = "Remove Iris"
    change_iris = "Change Iris"
    regist_iris = "Enable Iris"
    reset = "Reset the device"
    defi_Lock ="DeFi Lock"
    pin_management="PIN Management"
    iris_management="Iris Management"
    pin_keyboard="PIN Keyboard"
    authentication ="Device Authentication"
    screen_wake_up = "Screen wake-up"
    dynamic_effects = "Dynamic Effects"
    developer_options ="Developer options"
    about_device ="About the device"
    wallet = "Wallet"
    security = "Security"
    home_screen ="Home Screen"
    airgap_mode ="Air gap mode"
    verify_device ="Verify device"
    general =  "General"
    bluetooth = "Bluetooth"
    qr_code = "QR Code"
    language = "Language"
    vibration = "Vibration and Haptic Feedback"
    brightness = "Brightness"
    auto_lock = "Auto Lock"
    auto_shutdown = "Auto Shutdown"
    animation = "Transition Animation"
    wallpaper = "Wallpaper"
    power_off = "Power Off"
    restart = "Restart"
    input_on_device = "Input on device"
    auto_lock_and_shutdown = "Auto Lock/Shutdown"

# Security App
class Security:
    change_pin = "Change PIN"
    backup_mnemonic = "Backup Recovery Phrase"
    check_mnemonic = "Verify Recovery Phrase"
    wipe_device = "Factory Reset"

#### guide App
class Guide:
    quickstart = "Sealer2100 Quick Start"
    device_on_and_off = "Device Power On and Off"
    what_mnemonic = "What is a Mnemonic?"
    enable_pin_protection = "Enable PIN Protection"
    enable_iris_protection = "Enable Iris Protection"
    how_work = "How Hardware Wallet Works"
    passphrase = "Passphrase and Hidden Wallet"
    defi_lock = "DeFi Lock"
    need_help = "Need Help?"

    device_on_and_off_desc = "Press and hold the power button to turn the device on or off."
    what_mnemonic_desc = "A mnemonic is a human-readable format derived from a private key, making it easier to remember. It is the only key to recovering all your crypto assets. Keep it safe and never disclose it to anyone."
    enable_pin_protection_desc = "Set up a strong PIN code through system-level security mechanisms to provide comprehensive protection for your crypto assets."
    enable_iris_protection_desc = "After completing iris registration, you can unlock the device using your iris. When needed, simply use the front iris module to scan your registered eye."
    how_work_desc = "Sealer2100 encrypts and stores the mnemonic offline in a secure chip. It can only connect and interact with the HPX app via Bluetooth or Airgap."
    passphrase_desc = "A Passphrase can be understood as an extended mnemonic (mnemonic +1). By entering different Passphrases, you can access the corresponding hidden wallets."
    defi_lock_desc = "One click to disable DeFi transactions (while keeping transfers), defending against contract vulnerabilities and phishing risks, providing institutional-grade protection for your assets."


#### nft App
class Nft:
    nft_item ="{} item"
    nft_items ="{} items"
class Button:
    back ="Back"
    signature_transaction ="Signature transaction"
    enable ="Enable"
    disable = "Disable"
    delete ="Delete"
    reset = "Reset"
    set_home_screen="Set as home screen"
    iris_entry="Iris Entry"
    iris_not_now = "Not now"
    to_open="To open"
    verify_now = "Verify now"
    learn_more ="Learn more"
    mnemonic= "Mnemonic"
    metacard = "Sealer MetaCard"
    close = "Close"
    enable_pin_iris = "Use both PIN and iris verification "
    enable_pin = "Use PIN verification"
    enable_iris = "Use iris verification"
    create_wallet ="Create wallet"
    import_wallet = "Import wallet"
    done = "Done"
    ok = "OK"
    confirm = "Confirm"
    reject = "Reject"
    next = "Next"
    redo = "Regenerate"
    continue_ = "Continue"
    cancel = "Cancel"
    try_again = "Try again"
    power_off = "Power Off"
    restart = "Restart"
    hold = "Keep\nhold"
    address = "Address"
    qr_code = "QR Code"
    view_detail = "View Details"
    hold_to_sign = "Hold to\nsign"
    hold_to_wipe = "Hold to\nwipe"
    receive = "Receive Address"
    airgap = "Airgap"
    sign = "Sign"
    verify = "Verify"
    view_full_array = "View Full Array"
    view_full_struct = "View Full Structure"
    view_full_message = "View Full Message"
    view_data = "View Data"
    view_more = "View More"
    update = "Update"
    start = "Capture"

class Title:
    word_input ="Enter words #{}"
    iris_management="Iris management"
    airgap_mode_close="Disable Air Gap only"
    airgap_mode_open="Enable Air Gap only"
    connect_hpx_wallet ="Connect to HPX Wallet"
    connect_metamask_wallet ="Connect to MetaMask Wallet"
    connect_okx_wallet ="Connect to OKX web3 Wallet"
    learn_more = "Learn More"
    blue_connect = "Bluetooth connection"
    qr_connect = "QR code link"
    start_setting = "Start setting"
    link_wallet ="Link Wallet"
    download_app ="Download the App"
    word_check = "Word #{}"
    nearing_completion="Near Completion"
    start_backup = "Start backup"
    enable_protection='Enable protection'
    prepare_create='Prepare to create'
    prepare_recover='Prepare to recover'
    prepare_check='Prepare to check'
    create_wallet ="Create a new wallet"
    activate_device = "Activate Device"
    enter_old_pin = "Enter Current PIN"
    enter_new_pin = "Enter New PIN"
    enter_pin = "Enter PIN"
    enter_pin_again = "Re-enter PIN"
    select_language = "Select language"
    wallet = "Wallet"
    import_wallet = "Import Wallet"
    restore_wallet = "Restore Wallet"
    wallet_is_ready = "Wallet Ready"
    select_word_count = "Select Recovery Phrase Length"
    wallet_security = "Wallet Security"
    pin_security = "PIN Security Hint"
    mnemonic_security = "Recovery Phrase Security Hint"
    backup_mnemonic = "Backup Recovery Phrase"
    check_mnemonic = "Verify Recovery Phrase"
    enter_mnemonic = "Enter Recovery Phrase"
    success = "Success"
    operate_success = "Operation Successful"
    theme_success = "Theme Switched Successfully"
    warning = "Warning"
    error = "Error"
    verified = "Backup Complete"
    invalid_mnemonic = "Invalid Recovery Phrase"
    terminate_mnemonic ="Terminate reconciliation"
    pin_not_match = "Mismatch"
    check_recovery_mnemonic = "Verify Recovery Phrase"
    mnemonic_not_match = "Recovery Phrase Mismatch"
    power_off = "Power Off"
    restart = "Restart"
    change_language = "Change Language"
    wipe_device = "Factory Reset"
    bluetooth_pairing = "Bluetooth Pairing"
    address = "{} Address"
    public_key = "{} Public Key"
    xpub = "{} XPub"
    transaction = "{} Transaction"
    transaction_detail = "Transaction Details"
    confirm_transaction = "Confirm Transaction"
    confirm_message = "Confirm Message"
    signature = "Signature Details"
    wrong_pin = "Incorrect PIN"
    pin_changed = "PIN Changed"
    pin_enabled = "PIN Enabled"
    pin_disabled = "PIN Disabled"
    pin_removed = "PIN Removed"
    unknown_token = "Unknown Token"
    view_data = "View Data"
    sign_message = "{} Message Signing"
    verify_message = "{} Message Verification"
    typed_data = "{} Structured Data"
    typed_hash = "{} Structured Hash"
    system_update = "System Update"
    entering_boardloader = "Entering Boardloader"
    remove_credential = "Remove Credential"
    list_credentials = "List Credentials"
    authorize_coinjoin = "Authorize CoinJoin"
    multisig_address_m_of_n = "{} Multisig Address\n({} of {})"
    u2f_register = "U2F Registration"
    u2f_unregister = "U2F Deregistration"
    u2f_authenticate = "U2F Authentication"
    fido2_register = "FIDO2 Registration"
    fido2_unregister = "FIDO2 Deregistration"
    fido2_authenticate = "FIDO2 Authentication"
    finalize_transaction = "Finalize Transaction"
    meld_transaction = "Merge Transaction"
    update_transaction = "Update Transaction"
    high_fee = "High Fee"
    fee_is_high = "The handling fee is too high"
    confirm_locktime = "Confirm Locktime"
    view_transaction = "View Transaction"
    x_confirm_payment = "{} Confirm Payment"
    confirm_replacement = "Confirm Replacement Tx"
    x_transaction = "{} Transaction"
    x_joint_transaction = "{} Joint Transaction"
    change_label = "Edit Device Name"
    enable_passphrase = "Enable Passphrase"
    disable_passphrase = "Disable Passphrase"
    passphrase_source = "Passphrase Input Settings"
    enter_passphrase ="Enter Passphrase"
    enable_safety_checks = "Enable Safety Checks"
    disable_safety_checks = "Disable Safety Checks"
    experimental_mode =  "Experimental Mode",
    set_as_homescreen = "Set as Homescreen"
    collect_nft = "Collect this NFT"
    get_next_u2f_counter = "Get U2F Counter"
    set_u2f_counter = "Set U2F Counter"
    encrypt_value = "Encrypt Data"
    decrypt_value = "Decrypt Data"
    confirm_entropy = "Export Entropy"
    memo = "Memo"
    import_credential = "Import Credential"
    export_credential = "Export Credential"
    asset = "Asset"
    unimplemented = "Not Implemented"
    invalid_data = "Invalid Data Format"
    low_power = "Low Battery"
    collect_nft = "Collect NFT"
    verify_device ="Verify device"
    update_bootloader = "Update bootloader"
    update_resource = "Update resource"
    directions = "direction"
    send_amount = "send\n{}"
    reset_device ="Reset this device"
    device_protection_verification_failed = "Verification failed"
    correct = "Correct"
    spl_create_ata = "Create associated token account"
    select_derivation_path = "Select Derivation Path"

class Subtitle:
    account_qr_address ="Return to the App and scan the QR code below"
    confirm_transaction = "Are you sure you want to sign this transaction?"
    terminate_mnemonic ="Terminating this process will result in all progress being lost. Please confirm whether you want to terminate it."
    backup="You can choose from the following backup methods. Once the mnemonic is verified, the system will save it as a backup"
    enter_mnemonic ="Please enter the mnemonic words one by one according to the numbers in the backup, ensuring they are exactly the same."
    airgap_mode_close="Do you want to turn off Air Gap mode only? If you turn it off, you will be allowed to connect to this device via Bluetooth"
    airgap_mode_open="Are you sure to enable Air Gap mode only? After enabling, all operations such as connecting to software wallets and signing transactions will be completed in an offline environment by scanning the QR code"
    qr_connect_wallet_desc = "Open {} and scan the QR code below to support Ethereum and EVM networks."
    equipment_certification ="Are you sure you want to authenticate your device using the HPX secure server? Click the Confirm button to continue and ensure your device is authentic and has not been tampered with."
    learn_more = "Scan the QR code to view detailed tutorials"
    choose_connection_wallet ="Select the wallet you wish to connect."
    choose_connection_method = "Choose a connection method."
    start_setting = "Attach the Sealer MetaCard to the back of the device and then click Continue."
    import_wallet = "Select the import method you want"
    nft_count = "{} items"
    word_check = "Please choose the correct word"
    nearing_completion = "Please follow the prompts and check the mnemonics you backed up one by one."
    backup_mnemonic = "Copy the #00FE33 {}# words below in order."
    start_backup ="Next, you will be presented with a set of words called a mnemonic. Before you do, it is important to note the following:"
    wallet_is_ready = "New wallet created successfully!"
    enable_protection = "Keep your assets secure with a PIN or iris verification. Once turned on, all sensitive actions will be protected."
    select_mnemonics = "Select the number of mnemonics digits."
    activate_device = "Create a new wallet, or import an existing one to restore the wallet."
    create_wallet ="A standard wallet generated by a new set of mnemonic phrases is about to be created."
    reset_wallet = "Restore your device to factory settings."
class Tip:
    swipe_down_to_close = "Swipe Down to Close"
class Text:
    note = "Note"
    export_signed_transactions ="Export signed transactions"
    export_signed_transactions_desc ="Return to the APP and scan the QR code below"
    detail ="Detail"
    transaction_send = "Send {} {}"
    transaction_fee = "Fee"
    transaction_direction = "Send"
    transaction_from = "From"
    transaction_to = "To"
    gas_limit = "Gas limit"
    gas_fee_cap = "Gas Fee Cap"
    gas_premium = "Gas Premium"
    wallet_not_match= "Wallets don't match"
    wallet_not_match_desc= "The current hardware wallet does not match the wallet selected in the app. Please check whether the passphrase is entered incorrectly or the wallet is selected incorrectly."
    transaction_signed ="transaction signed"
    transaction_signed_desc ="Your transaction has been signed."
    invalid_transaction ="Invalid transaction"
    invalid_transaction_desc ="Invalid transaction data, please try again"
    scan_app="Scan the QR code displayed on the app"
    use_passphrase ="Use this passphrase？"
    use_passphrase_desc ="Please check the phrase you entered to make sure it is correct."
    enter_mnemonic_desc = "Please enter the words in order, making sure the numbers match your mnemonic backup exactly."
    limit_error ="Error limit reached"
    limit_error_desc ="The number of incorrect PIN codes has reached the limit. The device will reset in #28C600 5# seconds"
    backup_completed="Backup completed"
    backup_completed_desc="You have successfully backed up your mnemonic"
    MetaCard_pro="Sealer MetaCard"
    passphrase_disable ="Passphrase is disabled, currently only supports adding standard wallets"
    passphrase_disable_desc ="Do you want to disable the passphrase feature?"
    passphrase_enable ="The Passphrase function is enabled. Each time you connect to the Sealer2100 app, the device needs to enter the Passphrase. Please remember the Passphrase you set."
    passphrase_enable_desc="After enabling, you can add different hidden wallets based on different passphrases on the App."
    remove_pin_desc = "Are you sure you want to delete the PIN and only keep iris recognition?"
    enable_pin_ok ="You have successfully activated your PIN."
    enable_pin_titles =[
        "Strong PIN",
        "Store Properly"
    ]
    enable_pin_tips =[
        "Please use a strong PIN to prevent unauthorized access to your wallet.",
        "Keep your PIN safe and separate from your mnemonic phrase."
    ]
    keyboard_feedback="Keyboard tactile feedback"
    keyboard_feedback_desc ="Provide vibration feedback when interacting with the system"
    enable_pin ="Enable PIN protection"
    set_pin_length ="Set a PIN with a length of #28C600 4# to #28C600 16# digits to protect your wallet"
    reset_device_desc ="This operation will clear the device's internal storage and all data in the security chip (SE). Before continuing, please be aware of the following："
    reset_device_check = [
        "After the reset, your recovery phrase will be permanently deleted on this device",
        "You can still recover your wallet and assets through mnemonic backup"
    ]
    reset_warning = "Warning\nRestoring a hardware device to factory settings will erase all data on the device."
    defi_disable ="DeFi transactions are banned"
    defi_disable_desc ="During the DeFi lock period, all transactions except transfers will be prohibited"
    defi_lock_desc="Once the DeFi lock is turned on, all transactions except transfers will be prohibited."
    scan_learn_more_verify_device="Scan the QR code to learn more about verification methods"
    verify_device_desc = [
        {
            "title": "Download HPX Wallet",
            "message": "Download the HPX Wallet App for mobile from the following address: ",
        },
        {
            "title": "Verify device",
            "message": "Open the HPX Wallet App, enter Web3 and connect your device to create a wallet. Device verification will be done automatically.",
        }
    ]
    check_security_enable_desc ="When security checks are enabled, they will prevent the derivation of addresses that do not comply with the BIP-39 protocol and avoid executing transactions that have potential security risks or are too expensive."
    scan_learn_more= "Scan the QR code to learn more tutorials"
    system_update_desc=[
        {
            "title": "Download HPX Wallet",
            "message": "Download the HPX Wallet App for mobile from the following address: ",
        },
        {
            "title": "Start updateing",
            "message": 'Open the HPX Wallet App, go to "Web3 Wallet", select "Import Wallet" > "Connect Hardware Wallet" > "Sealer2100", then enter the wallet management, select the current hardware and enter the "System Update" in the management option.'
        }
    ]
    download_app = "Download the HPX app at:"
    serial_number = 'Serial number'
    model = 'Model'
    bluetooth_name = "Bluetooth name"
    system_version = 'System version'
    bluetooth_version = 'Bluetooth'
    bootloader="Bootloader"
    fcc="FCC"
    compatible="Compatible Trezor"
    system_update="System update"
    set_as_homescreen_desc = "Are you sure you want to set the current NFT as the home screen?"
    enable_pin= "Enable PIN"
    enable_pin_desc="You have not yet activated the PIN code. Do you want to activate the PIN code and enable two-factor authentication?"
    iris_regist = "Iris Registion"
    iris_match = "Iris Verification"
    iris_eye_focus = "Position your eye within the circle"
    iris_start_timeout = "Start Iris timeout"
    iris_removing_old = "Iirs removing old user"
    iris_changed="Iris Changed"
    iris_changed_ok="You have successfully changed your iris."
    iris_changed_failed="Iris change fialed"
    iris_removed = "Iris removed"
    iris_removed_ok = "You have successfully removed the iris."
    iris_removed_failed = "Failed to remove the iris."
    front_of_screen ="Please stay in front of the screen"
    near_of_screen="Please come closer"
    far_of_screen="Please step back a little"
    iris_regist_suggestion_distance_close = "Move back, enroll within 20–30 cm"
    iris_regist_suggestion_distance_far   = "Move closer, enroll within 20–30 cm"
    iris_match_suggestion_distance_close  = "Move back, identify within 20–30 cm"
    iris_match_suggestion_distance_far    = "Move closer, identify within 20–30 cm"
    iris_suggestion_registing="Enrolling, please do not move"
    iris_suggestion_matching="Identifying, please do not move"
    iris_operation_starting = "Starting ..."
    iris_operation_matching = "Matching ..."
    iris_operation_registing = "Registing ..."
    iris_operation_match_done = "Verify done"
    iris_operation_regist_done = "Regist done"
    iris_current_distance ="Current distance "
    new_iris_registration="New iris registration"
    new_iris_registration_desc="Start recording new irises for subsequent security verification."
    change_iris="Change iris"
    remove_iris="Remove iris"
    remove_iris_desc= "Deleting iris verification will significantly reduce the security level of your device. Keeping only PIN code verification may not provide sufficient protection. It is recommended to operate with caution"
    suggest_enable_iris = "For enhanced security, it is recommended to enroll iris information and enable both PIN code and iris dual protection."
    iris_verification ="Original iris verification"
    iris_verification_desc ="Verify the existing iris information on your device"
    change_iris_desc="Do you want to delete the currently registered iris information and re-enter a new iris?"
    register_iris_ok ="Iris registration successful"
    register_iris_error ="Iris registration failed"
    verify_iris_error = "Iris verification failed"
    verify_iris_error_desc = "Iris verification failed, please try again"
    register_iris_timed_out ="Iris entry timed out, please try again"
    iris_ok_desc ="Iris has been successfully added and can be used for security identification verification."
    iris_desc = "Please remove glasses, sunglasses, or contact lenses during registration to ensure enrollment with bare eyes."
    iris_entry_desc = "Such items may cause reflections or obstructions during iris registration, resulting in incomplete iris data and preventing the device from being unlocked later."
    register_iris="Register iris"
    register_iris_desc="The more secure iris verification is not currently enabled. Do you want to register your iris immediately and enable two-factor verification protection?"
    iris_duplicate = "Iris already registered"
    iris_duplicate_dsc = "The current iris is already registered and does not need to be changed."
    iris_note = "Please make sure you are not wearing any glasses, contact lenses, or other items that may block your eyes."
    random= "Random"
    default ="Default"
    pin_random="The numbers on the PIN pad are randomly arranged"
    pin_default="The numbers on the PIN pad are arranged in sequence."
    pin_changed ="You have successfully changed your PIN"
    airgap_mode_desc ="After enabling Air Gap only, the device's ability to connect to the App via Bluetooth will be disabled"
    airgap_mode = "Only supports Air Gap"
    screen_wake_up_tap="Tap to wake"
    screen_wake_up_button="Button to wake up"
    auto_shutdown_desc = "When the device screen is off, if there is no operation for {} consecutive minutes, the system will automatically shut down."
    auto_shutdown_never_desc =  "When the device screen is off, the system will not shut down automatically"
    screen_wake_up_tap_desc ="The screen is currently locked and can be woken up by touching the screen."
    screen_wake_up_button_desc ="The screen is currently locked. Press the power button to wake up the screen."
    dynamic_effects_disabled_desc = "UI animation disabled"
    dynamic_effects_allow_desc = "Allow dynamic effects in the user interface"
    vibration_desc ="Provides vibration feedback when interacting with the system"
    auto_lock_desc = "After {} continuous inactivity, the screen will automatically turn off and lock."
    auto_lock_never_desc =  "Do not automatically turn off the screen"
    metamask_connect_desc ="ETH and EVM Networks"

    blue_connect_hpx_wallet = [
        {
            "title": "Download HPX Wallet",
            "message": "Download the HPX Wallet App from the following address, mobile version: ",
        },
        {
            "title": "Connect via Bluetooth",
            "message": [
                "• Turn on your phone’s Bluetooth and keep the hardware wallet close to it.",
                "• Make sure the device is not in 'AirGap Only Mode'. This mode is disabled by default during initialization.",
                "• In the app, select the detected device name to complete the connection.",
                "#FFFFFF Tip:# If 'AirGap Only Mode' has been manually enabled, Bluetooth will be disabled. You must turn off this mode in the device settings before connecting the app via Bluetooth.",
            ]
        },
        {
            "title": "Pairing devices",
            "message": "Enter the pairing code of Sealer2100 in the HPX Wallet app and select ‘Start’ after verification."
        }
    ]

    blue_connect_okx_wallet = [
        {
            "title": "Access wallet",
            "message": 'Open the OKX mobile app for the first time, go to "Web3 Wallet", select "Import Wallet" > "Connect Hardware Wallet" > "Sealer2100"',
        },
        {
            "title": "Connect via Bluetooth",
            "message": "Turn on Bluetooth on both devices. Keep the wallet close to the phone and click the detected Sealer2100 device name."
        },
        {
            "title": "Import wallet account",
            "message": "After pairing is complete, enter the PIN code or iris recognition, wait for the wallet list to load, and then import the required account."
        }
    ]
    hpx_wallet = "HPX Wallet"
    okx_wallet = "OKX Wallet"
    in_connection ="In connection..."
    in_connection_desc ="Keep the Sealer MetaCard together with the device until the transmission is completed."
    mismatch ="Mismatch"
    mismatch_desc ="The Sealer MetaCard PIN code you entered does not match. Please try again."
    need_help ="Need help ?"
    need_help_desc ="If you have any questions that need answers, please go to the HPX Help Center for assistance."
    swipe_up_to_apps ="Swipe up to view apps"
    connect_wallet_1 = "Click “Connect Hardware Wallet.”"
    connect_wallet_2 = "Connect to the device \"{}\"."
    connect_wallet_3 = "After completing the guided steps, the wallet account in your hardware device will be restored to the HPX Web3 wallet list."
    word_error = "Word Error"
    word_error_desc = "The word was entered incorrectly, please check your backup and try again."
    verification_passed = "Verification passed"
    verification_passed_desc = "Mnemonic verification completed."
    correct = "Correct!"
    incorrect = "Incorrect!"
    backup_title = [
        "Store Properly",
        "Avoid photo",
        "Keep it safe"
    ]
    backup_check = [
        "If you lose your mnemonic phrase, you won’t be able to recover your wallet or assets.",
        "Mnemonic phrases must not be photographed, digitally copied, or uploaded online in any way to avoid the risk of leakage.",
        "Please record the mnemonic and store it in a safe place. Do not share or forward it with others."
    ]
    recommend = "recommend"
    please_wait = "Please wait..."
    my_wallet = "My Wallet"
    tap_to_unlock = "Touch the screen to unlock"
    swipe_to_view_app = "Swipe up to view the apps"
    processing = "Processing, please wait..."
    signing = "Building transaction..."
    loading = "Loading transaction..."
    unlocking = "Unlocking device..."
    str_words = "{} words"
    backup_manual = "Manually write down recovery phrase and store securely"
    check_manual = "Tap words below in correct order"
    backup_verified = "Recovery phrase backup complete. Store securely and never share"
    backup_invalid = "Invalid recovery phrase. Please verify and try again"
    pin_not_match = "Incorrect PIN. Please try again"
    pin_error = "Incorrect PIN entered"
    wiping_device = "Wiping device data..."
    create_wallet = "Generate a new recovery phrase to create wallet"
    restore_wallet = "Restore wallet from your backup phrase"
    restore_mnemonic_match = "Recovery phrase matches. Backup is valid"
    restore_success = "Wallet Restored Successfully"
    create_success = "Recovery phrase backed up successfully. Wallet created"
    check_recovery_mnemonic = "Verify your recovery phrase matches exactly."
    invalid_recovery_mnemonic = "Invalid recovery phrase. Please verify and try again"
    valid_recovery_mnemonic = "The mnemonic you entered matches exactly, your backup is correct."
    check_recovery_not_match = "Valid recovery phrase but doesn't match device"
    shutting_down = "Shutting down..."
    restarting = "Restarting..."
    never = "Never"
    second = "Second"
    seconds = "Seconds"
    minute = "Minute"
    minutes = "Minutes"
    changing_language = "Changing language\nDevice will restart"
    change_pin = "Set a #00FE33 4#~#00FE33 16# digit PIN to secure your device"
    wipe_device = "Reset device to factory settings.\nWARNING: This erases ALL data."
    wipe_device_check = [
        "Factory reset erases all data",
        "Data cannot be recovered",
        "Recovery phrase is backed up",
    ]
    wipe_device_success = "Device wiped successfully\nRestarting..."
    bluetooth_pair = "Enter pairing code on your device"
    bluetooth_pair_failed = "Bluetooth Pairing Failed"
    path = "Derivation Path:"
    chain_id = "Chain ID:"
    send = "Send"
    to = "To"
    amount = "Amount"
    from_ = "From"
    receiver = "Recipient"
    fee = "Fee"
    max_fee = "Max Fee"
    max_priority_fee_per_gas = "Max Priority Fee"
    max_fee_per_gas = "Max Fee per Gas"
    max_gas_limit = "Max gas limit:"
    gas_unit_price= "Gas unit price:"
    gas_price = "Gas Price"
    total = "Total"
    do_sign_this_transaction = "Sign this{}transaction?"
    transaction_signed = "Transaction signed"
    address = 'Address:'
    public_key = 'Public Key:'
    xpub = "XPub:"
    unknown_tx_type = "Unknown transaction type. Check input data"
    unknown_function = "Unknown function:"
    use_app_scan_this_signature = "Scan signature with Digital Shield app"
    internal_error = "Internal Error"
    tap_switch_to_airgap = "Tap QR code to show Airgap address"
    tap_switch_to_receive = "Tap QR code to show receive address"
    incorrect_pin_times_left = "Incorrect PIN. {} attempts remaining"
    incorrect_pin_last_time = "Incorrect PIN.Last attempt"
    wrong_pin = "Incorrect PIN"
    seedless = "No seed phrase detected"
    backup_failed = "Backup Failed!"
    need_backup = "Backup Required!"
    pin_not_set = "No PIN Set!"
    experimental_mode = "Experimental Mode"
    pin_change_success = "PIN Changed Successfully"
    pin_enable_success = "PIN Enabled Successfully"
    pin_disable_success = "PIN Disabled Successfully"
    pin_remove_success = "PIN Remove Successfully"
    contract = "Contract:"
    new_contract = "New contract?"
    bytes_ = "{} bytes"
    message = "Message:"
    no_message = "No message included"
    contains_x_key = "Contains {} key(s)"
    array_of_x_type = "Array type {} {}"
    do_sign_712_typed_data = "Sign this structured data transaction？"
    do_sign_typed_hash = "Sign this structured hash transaction?"
    domain_hash = "Domain Hash:"
    message_hash = "Message  Hash:"
    switch_to_update_mode = "Switch to Update Mode"
    switch_to_boardloader = "Switch to Boardloader Mode"
    list_credentials = "Export credentials stored on this device？"
    coinjoin_at_x = "participate in the following CoinJoin transaction:\n{}?"
    signature_is_valid = "Valid Signature"
    signature_is_invalid = "Signature Invalid"
    u2f_already_registered = "U2F Already Registered"
    u2f_not_registered = "U2F Not Registered"
    fido2_already_registered_x = "FIDO2 Registered {}"
    fido2_verify_user = "FIDO2 Verify User"
    device_already_registered_x = "Device Registered {}"
    device_verify_user = "Device-Verified User"
    fee_is_unexpectedly_high = "Unexpectedly High Transaction Fee"
    too_many_change_outputs= "There are too many change outputs"
    change_count = "Change Output Count"
    locktime_will_have_no_effect = "Locktime Will Have No Effect"
    confirm_locktime_for_this_transaction = "Confirm Transaction Locktime"
    block_height = "Block Height"
    time = "Time"
    amount_increased = "Amount Increased"
    amount_decreased = "Amount Decreased"
    fee_unchanged = "Fee Unchanged"
    fee_increased = "Fee Increased"
    fee_decreased = "Fee Decreased"
    your_spend = "Your Spend"
    change_label_to_x = "Change Label to {}"
    enable_passphrase = "Enable Passphrase Encryption?"
    disable_passphrase = "Disable Passphrase Encryption?"
    enable_passphrase_always = "Always Enter Passphrase On Device?"
    revoke_enable_passphrase_always = "Revoke On-Device Passphrase Entry?"
    auto_lock_x = "Auto-Lock Device After {}?"
    enable_safety_checks = "Enable strict safety checks for enhanced security?"
    disable_safety_checks = "Disable safety checks? Understand security risks."
    experimental_mode = "Experimental Mode"
    set_as_homescreen = "Set as Homescreen?"
    replace_homescreen = "Replace Homescreen? Previous wallpaper will be deleted."
    confirm_replace_wallpaper = "Are you sure you want to replace the wallpaper of the home screen?"
    get_next_u2f_counter = "Get Next U2F Counter?"
    set_u2f_counter_x = "Set U2F Counter to ?"
    confirm_entropy = "Export Entropy? Understand the risks before proceeding."
    bandwidth = "Bandwidth"
    energy = "Energy"
    sender = "Sender"
    recipient = "Recipient"
    resource = "Resource"
    frozen_balance = "Frozen Balance"
    unfrozen_balance = "Unfrozen Balance"
    delegated_balance = "Delegated Balance"
    undelegated_balance = "Undelegated Balance"
    you_are_freezing = "Freezing Assets"
    you_are_unfreezing = "Unfreezing Assets"
    you_are_delegating = "Delegating Assets"
    you_are_undelegating = "Undelegating Assets"
    duration = "Duration"
    lock = "Lock"
    unlock = "Unlock"
    all = "All"
    source = "Source"
    tip = "Tip"
    keep_alive = "Keep Alive"
    invalid_ur = "Unsupported QR Type. Please Retry."
    sequence_number = "Serial Number"
    expiration_time = "Expiration Time"
    argument_x = "Argument #{}"
    low_power_message = "Battery at {}%\nPlease charge"
    collect_nft = "Are you sure collect the NFT?"
    replace_nft = "Do you want to collect this NFT? You have reach the storage limit, this will remove the oldest uploaded NFT."
    verify_device = "Are you sure to authenticate your device with the Sealer2100 server? Tap confirm to check whether your device is genuine and without tampered."
    update_bootloader = "Do you want to update bootloader?"
    update_resource = "Do you want to update device resource?"
    need_input_tips = "Please enter all the mnemonic words before clicking the next step"
    need_select_tips = "Please click and select in the order of the mnemonic words, and then click the next step after completion."
    return_check_mnemonic = "Check the mnemonic words"
    device_wakeup_by_touch_screen = "By touch screen"
    device_wakeup_by_click_power_button = "By click power button"
    on = "On"
    off = "Off"
    input_passphrase_on_device_desc = "Input on device when request Passphrase"
    input_passphrase_on_host_desc = "Input on APP when request Passphrase"
    already_input = "Already input"
    please_backup = "Please back up your new wallet immediately."

    ## iris error code
    iris_code_0 = "Success"
    iris_code_1 = "Liveness detection failed"
    iris_code_2 = "Module rejected the command"
    iris_code_3 = "Timeout failure"
    iris_code_4 = "Iris already registered"
    iris_code_5 = "User ID already exists"
    iris_code_6 = "No users registered, no users in module"
    iris_code_7 = "User ID does not exist"
    iris_code_8 = "Maximum number of users reached in module"
    iris_code_11 = "Registration/recognition process terminated"
    iris_code_12 = "Parameter error"
    iris_code_13 = "Camera failed to open"
    iris_code_14 = "Module initialization failed"
    iris_code_15 = "Liveness detection failed"

    ## iris notes message
    iris_note_module_status_1 = "Module started successfully"
    iris_note_module_status_2 = "Module started recognition"
    iris_note_module_status_3 = "Module started registration"

    iris_note_regist_progress = "Registration progress: {} \t {}"

    iris_note_iris_status_0 = "Iris normal"
    iris_note_iris_status_1 = "Too close"
    iris_note_iris_status_2 = "Too far"
    iris_note_iris_status_3 = "Iris not detected"
    device_protection_verification_failed_msg = "PIN code or iris does not match. Please try again."
    device_protection_verification_left = "{} attempts remaining"
    power_off_confirm = "Are you sure you want to power off?"
    too_many_iris_match_count = "Too many iris match timeouts\n\nDo you want to erase the device?"
    evm_address_desc = "This address is a universal EVM address and can be used to manage assets on other EVM-compatible chains (such as Ethereum, BNB Chain, Avalanche, Arbitrum, Optimism, Polygon, etc.) simultaneously."

class WalletSecurity:
    header = "Write recovery phrase on paper and store securely"
    tips = [
        {
            "level": "info",
            "msgs": [
                "#00001F Securely store your recovery phrase#",
                "#18794E * Store in a bank safety deposit box#",
                "#18794E * Store it in a home safe#",
                "#18794E * Store in multiple secret locations#",
            ]
        },
        {
            "level": "warning",
            "msgs": [
                "#00001F Critical Warnings#",
                "#CD2B31 * Memorize recovery phrase#",
                "#CD2B31 * Never lose it#",
                "#CD2B31 * Never share it#",
                "#CD2B31 * Never store it online#",
                "#CD2B31 * Never store it on your computer#",
            ]
        },
    ]

class MnemonicSecurity:
    header = "Recovery phrase restores wallet assets. Whoever possesses it controls your assets. Store securely."

    tips = [
        "1. Ensure private environment without observers/cameras",
        "2. Backup words in correct order. Never share your phrase",
        "3. Store offline securely. Never backup digitally or upload online",
    ]

class PinSecurity:
    header = "PIN authorizes device access. Follow these security practices:"
    tips = [
        "1.Ensure your environment is safe when setting or entering your PIN — avoid bystanders or surveillance."
        "2.Choose a strong PIN (4–16 digits). Avoid patterns like repeated or sequential numbers.",
        "3.You have up to 10 attempts. The device will reset after 10 incorrect entries.",
        "4.Keep your PIN confidential. Never share it with anyone.",
    ]

class Solana:
    ata_reciver = "Receiver (Associated Token Account)"
    ata_sender = "Sender (Associated Token Account)"
    source_owner = "Transaction Signer"
    fee_payer = "Fee Payer"
    associated_token_account = "Associated token account"
    owner = "Owner"
    mint_address = "Mint address"
    found_by = "Found by"
    blinding_sign = "Solana raw sign"
    blinding_sign_dsc = "Allows signing raw Solana messages without processing or validation. This may expose you to phishing, blind signing, and unauthorized approvals. Use with caution."

class Filecoin:
    gas_limit = "Gas Limit"
    gas_fee_cap = "Gas Fee Cap"
    gas_premium = "Gas Premium"

class Ripple:
    destination_tag = "Destination Tag"

