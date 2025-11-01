//部分功能由AI生成
#include <QApplication>
#include <QMainWindow>
#include <QTextEdit>
#include <QDockWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QFont>
#include <QFontDatabase>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QStringListModel>
#include <QListView>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QDebug>
#include <QTime>
#include <QTextCursor>
#include <QTextBlock>
#include <QScrollBar>
#include <QDir>
#include <QStandardPaths>
#include <QComboBox>
#include <QLabel>
#include <QThread>
#include <QTextStream>
#include <QDesktopServices>
#include <QPair>
#include <QList>
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDialog>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextDocument>
#include <QTextBlock>
#include <QStyleFactory>
#include <QPalette>
#include <QTabWidget>
#include <QSplitter>
#include <QTreeView>
#include <QFileSystemModel>
#include <QToolButton>
#include <QInputDialog>
#include <QCollator>
#include <QClipboard>
#include <QMimeData>
#include <QShortcut>
#include <QTextDocumentFragment>
#include <QTextBlockUserData>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QProxyStyle>
#include <QCommonStyle>
#include <windows.h>
#include <shellapi.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDirIterator>
#include <QStandardPaths>
#include <QListWidget>
#include <QProgressBar>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QCryptographicHash>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QRadioButton>

const QString IDE_VERSION = "2.8.9";
const int VERSION_MAJOR = 2;
const int VERSION_MINOR = 8;
const int VERSION_PATCH = 9;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#endif

const QString ACCOUNTS_FILE = "accounts.dat";
const QString SESSION_FILE = "session.dat";

class ModernStyle : public QProxyStyle
{
public:
    ModernStyle() : QProxyStyle("Fusion") {}
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget) const override
    {
        if (element == PE_FrameFocusRect)
            return;
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
    int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const override
    {
        switch (metric)
        {
        case PM_SplitterWidth:
            return 6;
        case PM_TabBarTabHSpace:
            return 20;
        case PM_TabBarTabVSpace:
            return 10;
        default:
            return QProxyStyle::pixelMetric(metric, option, widget);
        }
    }
};

// 用户账户类
class UserAccount {
public:
    QString username;
    QString passwordHash; // 存储密码的哈希值
    QString email;
    QDateTime registerDate;
    QDateTime lastLogin;
    bool isAdmin;

    UserAccount() : isAdmin(false) {}
    UserAccount(const QString &user, const QString &pwd, const QString &mail = "")
        : username(user), email(mail), isAdmin(false)
    {
        setPassword(pwd);
        registerDate = QDateTime::currentDateTime();
        lastLogin = QDateTime::currentDateTime();
    }

    void setPassword(const QString &password) {
        passwordHash = hashPassword(password);
    }

    bool checkPassword(const QString &password) const {
        return passwordHash == hashPassword(password);
    }

    static QString hashPassword(const QString &password) {
        QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(password.toUtf8());
        return hash.result().toHex();
    }

    // 序列化到QVariant
    QVariant toVariant() const {
        QVariantMap map;
        map["username"] = username;
        map["passwordHash"] = passwordHash;
        map["email"] = email;
        map["registerDate"] = registerDate;
        map["lastLogin"] = lastLogin;
        map["isAdmin"] = isAdmin;
        return map;
    }

    // 从QVariant反序列化
    static UserAccount fromVariant(const QVariant &variant) {
        UserAccount account;
        QVariantMap map = variant.toMap();
        account.username = map["username"].toString();
        account.passwordHash = map["passwordHash"].toString();
        account.email = map["email"].toString();
        account.registerDate = map["registerDate"].toDateTime();
        account.lastLogin = map["lastLogin"].toDateTime();
        account.isAdmin = map["isAdmin"].toBool();
        return account;
    }
};

// 账户管理器类
class AccountManager : public QObject {
    Q_OBJECT

public:
    explicit AccountManager(QObject *parent = nullptr);
    ~AccountManager();

    bool registerAccount(const QString &username, const QString &password,
                         const QString &email = "", QString *error = nullptr);
    bool login(const QString &username, const QString &password,
               QString *error = nullptr);
    void logout();
    bool changePassword(const QString &username, const QString &oldPassword,
                        const QString &newPassword, QString *error = nullptr);
    bool deleteAccount(const QString &username, const QString &password,
                       QString *error = nullptr);

    bool isLoggedIn() const { return currentUser != nullptr; }
    QString getCurrentUsername() const {
        return currentUser ? currentUser->username : "";
    }
    UserAccount* getCurrentUser() const { return currentUser; }
    bool isAdmin() const { return currentUser ? currentUser->isAdmin : false; }

    QList<UserAccount> getAllAccounts() const { return accounts.values(); }
    bool usernameExists(const QString &username) const {
        return accounts.contains(username);
    }

signals:
    void loginStatusChanged(bool loggedIn);
    void userLoggedIn(const QString &username);
    void userLoggedOut();

private:
    void loadAccounts();
    void saveAccounts();
    void loadSession();
    void saveSession();
    bool validateUsername(const QString &username) const;
    bool validatePassword(const QString &password) const;
    bool validateEmail(const QString &email) const;

    QMap<QString, UserAccount> accounts;
    UserAccount *currentUser;
};

AccountManager::AccountManager(QObject *parent)
    : QObject(parent), currentUser(nullptr)
{
    loadAccounts();
    loadSession();
}

AccountManager::~AccountManager()
{
    saveAccounts();
    saveSession();
    delete currentUser;
}

void AccountManager::loadAccounts()
{
    QFile file(ACCOUNTS_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        // 如果文件不存在，创建默认管理员账户
        UserAccount admin("admin", "admin123", "admin@compileride.com");
        admin.isAdmin = true;
        accounts[admin.username] = admin;
        saveAccounts();
        return;
    }

    QDataStream in(&file);
    QVariantList accountList;
    in >> accountList;

    accounts.clear();
    for (const QVariant &variant : accountList) {
        UserAccount account = UserAccount::fromVariant(variant);
        accounts[account.username] = account;
    }
}

void AccountManager::saveAccounts()
{
    QFile file(ACCOUNTS_FILE);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法保存账户文件:" << file.errorString();
        return;
    }

    QDataStream out(&file);
    QVariantList accountList;
    for (const UserAccount &account : accounts) {
        accountList.append(account.toVariant());
    }
    out << accountList;
}

void AccountManager::loadSession()
{
    QFile file(SESSION_FILE);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QDataStream in(&file);
    QString username;
    in >> username;

    if (accounts.contains(username)) {
        currentUser = new UserAccount(accounts[username]);
        currentUser->lastLogin = QDateTime::currentDateTime();
        emit loginStatusChanged(true);
        emit userLoggedIn(username);
    }
}

void AccountManager::saveSession()
{
    QFile file(SESSION_FILE);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QDataStream out(&file);
    out << (currentUser ? currentUser->username : "");
}

bool AccountManager::validateUsername(const QString &username) const
{
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }

    // 只允许字母、数字和下划线
    QRegularExpression regex("^[a-zA-Z0-9_]+$");
    return regex.match(username).hasMatch();
}

bool AccountManager::validatePassword(const QString &password) const
{
    return password.length() >= 6 && password.length() <= 30;
}

bool AccountManager::validateEmail(const QString &email) const
{
    if (email.isEmpty()) return true; // 邮箱可选

    QRegularExpression regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex.match(email).hasMatch();
}

bool AccountManager::registerAccount(const QString &username, const QString &password,
                                     const QString &email, QString *error)
{
    if (!validateUsername(username)) {
        if (error) *error = "用户名必须为3-20个字符，且只能包含字母、数字和下划线";
        return false;
    }

    if (!validatePassword(password)) {
        if (error) *error = "密码长度必须在6-30个字符之间";
        return false;
    }

    if (!validateEmail(email)) {
        if (error) *error = "邮箱格式不正确";
        return false;
    }

    if (usernameExists(username)) {
        if (error) *error = "用户名已存在";
        return false;
    }

    UserAccount newAccount(username, password, email);
    accounts[username] = newAccount;
    saveAccounts();

    qDebug() << "新用户注册:" << username;
    return true;
}

bool AccountManager::login(const QString &username, const QString &password,
                           QString *error)
{
    if (!accounts.contains(username)) {
        if (error) *error = "用户名不存在";
        return false;
    }

    UserAccount &account = accounts[username];
    if (!account.checkPassword(password)) {
        if (error) *error = "密码错误";
        return false;
    }

    // 更新最后登录时间
    account.lastLogin = QDateTime::currentDateTime();
    saveAccounts();

    // 设置当前用户
    delete currentUser;
    currentUser = new UserAccount(account);

    emit loginStatusChanged(true);
    emit userLoggedIn(username);

    qDebug() << "用户登录:" << username;
    return true;
}

void AccountManager::logout()
{
    if (currentUser) {
        QString username = currentUser->username;
        delete currentUser;
        currentUser = nullptr;

        emit loginStatusChanged(false);
        emit userLoggedOut();

        qDebug() << "用户登出:" << username;
    }
}

bool AccountManager::changePassword(const QString &username, const QString &oldPassword,
                                    const QString &newPassword, QString *error)
{
    if (!accounts.contains(username)) {
        if (error) *error = "用户名不存在";
        return false;
    }

    UserAccount &account = accounts[username];
    if (!account.checkPassword(oldPassword)) {
        if (error) *error = "原密码错误";
        return false;
    }

    if (!validatePassword(newPassword)) {
        if (error) *error = "新密码长度必须在6-30个字符之间";
        return false;
    }

    account.setPassword(newPassword);
    saveAccounts();

    // 如果修改的是当前登录用户的密码，更新currentUser
    if (currentUser && currentUser->username == username) {
        currentUser->setPassword(newPassword);
    }

    return true;
}

bool AccountManager::deleteAccount(const QString &username, const QString &password,
                                   QString *error)
{
    if (!accounts.contains(username)) {
        if (error) *error = "用户名不存在";
        return false;
    }

    UserAccount &account = accounts[username];
    if (!account.checkPassword(password)) {
        if (error) *error = "密码错误";
        return false;
    }

    // 不能删除当前登录的账户
    if (currentUser && currentUser->username == username) {
        logout();
    }

    accounts.remove(username);
    saveAccounts();

    return true;
}

class RegisterDialog : public QDialog {
    Q_OBJECT

public:
    explicit RegisterDialog(AccountManager *accountManager, QWidget *parent = nullptr);
    ~RegisterDialog();

    QString getUsername() const { return usernameEdit->text().trimmed(); }

private slots:
    void onRegisterClicked();
    void onTextChanged();

private:
    void setupUI();
    void updateRegisterButton();

    AccountManager *accountManager;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLineEdit *emailEdit;
    QPushButton *registerButton;
    QLabel *statusLabel;
};

RegisterDialog::RegisterDialog(AccountManager *accountManager, QWidget *parent)
    : QDialog(parent), accountManager(accountManager)
{
    setWindowTitle("注册新账户 - Compiler IDE");
    setModal(true);
    setFixedSize(450, 350);

    setupUI();

    // 连接信号
    connect(registerButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &RegisterDialog::onTextChanged);
    connect(passwordEdit, &QLineEdit::textChanged, this, &RegisterDialog::onTextChanged);
    connect(confirmPasswordEdit, &QLineEdit::textChanged, this, &RegisterDialog::onTextChanged);
    connect(emailEdit, &QLineEdit::textChanged, this, &RegisterDialog::onTextChanged);

    updateRegisterButton();
}

RegisterDialog::~RegisterDialog()
{
}

void RegisterDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("注册新账户");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(20);

    // 表单
    QFormLayout *formLayout = new QFormLayout();

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("3-20个字符，字母、数字、下划线");
    formLayout->addRow("用户名*:", usernameEdit);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("至少6个字符");
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码*:", passwordEdit);

    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("再次输入密码");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("确认密码*:", confirmPasswordEdit);

    emailEdit = new QLineEdit();
    emailEdit->setPlaceholderText("可选，用于找回密码");
    formLayout->addRow("邮箱:", emailEdit);

    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(20);

    // 状态标签
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(statusLabel);

    mainLayout->addStretch();

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *cancelButton = new QPushButton("取消");
    registerButton = new QPushButton("注册");
    registerButton->setDefault(true);

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(registerButton);

    mainLayout->addLayout(buttonLayout);

    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void RegisterDialog::onRegisterClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    QString email = emailEdit->text().trimmed();

    if (password != confirmPassword) {
        statusLabel->setText("两次输入的密码不一致");
        return;
    }

    QString error;
    if (accountManager->registerAccount(username, password, email, &error)) {
        accept();
    } else {
        statusLabel->setText(error);
    }
}

void RegisterDialog::onTextChanged()
{
    updateRegisterButton();
    statusLabel->clear();
}

void RegisterDialog::updateRegisterButton()
{
    bool enabled = !usernameEdit->text().trimmed().isEmpty() &&
                   !passwordEdit->text().isEmpty() &&
                   !confirmPasswordEdit->text().isEmpty();
    registerButton->setEnabled(enabled);
}

// 登录对话框
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(AccountManager *accountManager, QWidget *parent = nullptr);
    ~LoginDialog();

    QString getUsername() const { return username; }
    bool isLoginSuccessful() const { return loginSuccess; }

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onUsernameChanged(const QString &text);
    void onPasswordChanged(const QString &text);
    void updateLoginButton();

private:
    void setupUI();

    AccountManager *accountManager;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QLabel *statusLabel;

    QString username;
    bool loginSuccess;
};

LoginDialog::LoginDialog(AccountManager *accountManager, QWidget *parent)
    : QDialog(parent), accountManager(accountManager), loginSuccess(false)
{
    setWindowTitle("登录 - Compiler IDE");
    setModal(true);
    setFixedSize(400, 300);

    setupUI();

    // 连接信号
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(usernameEdit, &QLineEdit::textChanged, this, &LoginDialog::onUsernameChanged);
    connect(passwordEdit, &QLineEdit::textChanged, this, &LoginDialog::onPasswordChanged);

    updateLoginButton();
}

LoginDialog::~LoginDialog()
{
}

void LoginDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("Compiler IDE 账户登录");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(20);

    // 表单
    QFormLayout *formLayout = new QFormLayout();

    usernameEdit = new QLineEdit();
    usernameEdit->setPlaceholderText("输入用户名");
    formLayout->addRow("用户名:", usernameEdit);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("密码:", passwordEdit);

    mainLayout->addLayout(formLayout);

    mainLayout->addSpacing(20);

    // 状态标签
    statusLabel = new QLabel();
    statusLabel->setWordWrap(true);
    statusLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(statusLabel);

    mainLayout->addStretch();

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    loginButton = new QPushButton("登录");
    loginButton->setDefault(true);
    registerButton = new QPushButton("注册新账户");

    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);

    mainLayout->addLayout(buttonLayout);
}

void LoginDialog::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();
    QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        statusLabel->setText("请输入用户名和密码");
        return;
    }

    QString error;
    if (accountManager->login(username, password, &error)) {
        this->username = username;
        loginSuccess = true;
        accept();
    } else {
        statusLabel->setText(error);
    }
}

void LoginDialog::onRegisterClicked()
{
    // 打开注册对话框
    RegisterDialog dialog(accountManager, this);
    if (dialog.exec() == QDialog::Accepted) {
        statusLabel->setText("注册成功！请使用新账户登录");
        usernameEdit->setText(dialog.getUsername());
        passwordEdit->clear();
        passwordEdit->setFocus();
    }
}

void LoginDialog::onUsernameChanged(const QString &text)
{
    Q_UNUSED(text)
    updateLoginButton();
    statusLabel->clear();
}

void LoginDialog::onPasswordChanged(const QString &text)
{
    Q_UNUSED(text)
    updateLoginButton();
    statusLabel->clear();
}

void LoginDialog::updateLoginButton()
{
    bool enabled = !usernameEdit->text().trimmed().isEmpty() &&
                   !passwordEdit->text().isEmpty();
    loginButton->setEnabled(enabled);
}

// 用户信息对话框
class UserInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit UserInfoDialog(AccountManager *accountManager, QWidget *parent = nullptr);
    ~UserInfoDialog();

private slots:
    void onChangePasswordClicked();
    void onDeleteAccountClicked();
    void refreshUserInfo();

private:
    void setupUI();

    AccountManager *accountManager;
    QLabel *usernameLabel;
    QLabel *emailLabel;
    QLabel *registerDateLabel;
    QLabel *lastLoginLabel;
    QLabel *userTypeLabel;
    QPushButton *changePasswordButton;
    QPushButton *deleteAccountButton;
};

UserInfoDialog::UserInfoDialog(AccountManager *accountManager, QWidget *parent)
    : QDialog(parent), accountManager(accountManager)
{
    setWindowTitle("用户信息 - Compiler IDE");
    setModal(true);
    setFixedSize(400, 300);

    setupUI();
    refreshUserInfo();

    connect(changePasswordButton, &QPushButton::clicked,
            this, &UserInfoDialog::onChangePasswordClicked);
    connect(deleteAccountButton, &QPushButton::clicked,
            this, &UserInfoDialog::onDeleteAccountClicked);
}

UserInfoDialog::~UserInfoDialog()
{
}

void UserInfoDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 标题
    QLabel *titleLabel = new QLabel("用户信息");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(20);

    // 用户信息
    QFormLayout *infoLayout = new QFormLayout();

    usernameLabel = new QLabel();
    emailLabel = new QLabel();
    registerDateLabel = new QLabel();
    lastLoginLabel = new QLabel();
    userTypeLabel = new QLabel();

    infoLayout->addRow("用户名:", usernameLabel);
    infoLayout->addRow("邮箱:", emailLabel);
    infoLayout->addRow("注册时间:", registerDateLabel);
    infoLayout->addRow("最后登录:", lastLoginLabel);
    infoLayout->addRow("用户类型:", userTypeLabel);

    mainLayout->addLayout(infoLayout);

    mainLayout->addStretch();

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    changePasswordButton = new QPushButton("修改密码");
    deleteAccountButton = new QPushButton("删除账户");
    QPushButton *closeButton = new QPushButton("关闭");

    buttonLayout->addWidget(changePasswordButton);
    buttonLayout->addWidget(deleteAccountButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void UserInfoDialog::refreshUserInfo()
{
    UserAccount *user = accountManager->getCurrentUser();
    if (!user) return;

    usernameLabel->setText(user->username);
    emailLabel->setText(user->email.isEmpty() ? "未设置" : user->email);
    registerDateLabel->setText(user->registerDate.toString("yyyy-MM-dd hh:mm:ss"));
    lastLoginLabel->setText(user->lastLogin.toString("yyyy-MM-dd hh:mm:ss"));
    userTypeLabel->setText(user->isAdmin ? "管理员" : "普通用户");
}

void UserInfoDialog::onChangePasswordClicked()
{
    // 修改密码对话框
    QDialog dialog(this);
    dialog.setWindowTitle("修改密码");
    dialog.setFixedSize(350, 200);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QFormLayout *formLayout = new QFormLayout();
    QLineEdit *oldPasswordEdit = new QLineEdit();
    oldPasswordEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *newPasswordEdit = new QLineEdit();
    newPasswordEdit->setEchoMode(QLineEdit::Password);
    QLineEdit *confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("原密码:", oldPasswordEdit);
    formLayout->addRow("新密码:", newPasswordEdit);
    formLayout->addRow("确认新密码:", confirmPasswordEdit);

    layout->addLayout(formLayout);

    QLabel *statusLabel = new QLabel();
    statusLabel->setStyleSheet("color: red;");
    layout->addWidget(statusLabel);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString oldPassword = oldPasswordEdit->text();
        QString newPassword = newPasswordEdit->text();
        QString confirmPassword = confirmPasswordEdit->text();

        if (newPassword != confirmPassword) {
            statusLabel->setText("新密码不一致");
            return;
        }

        QString error;
        if (accountManager->changePassword(accountManager->getCurrentUsername(),
                                           oldPassword, newPassword, &error)) {
            QMessageBox::information(this, "成功", "密码修改成功");
        } else {
            QMessageBox::warning(this, "错误", error);
        }
    }
}

void UserInfoDialog::onDeleteAccountClicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "确认删除",
                                 "确定要删除您的账户吗？此操作不可恢复！",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 输入密码确认
        QString password = QInputDialog::getText(this, "确认删除",
                                                 "请输入密码确认:",
                                                 QLineEdit::Password);

        if (password.isEmpty()) return;

        QString error;
        if (accountManager->deleteAccount(accountManager->getCurrentUsername(),
                                          password, &error)) {
            QMessageBox::information(this, "成功", "账户已删除");
            accept();
        } else {
            QMessageBox::warning(this, "错误", error);
        }
    }
}

class FoldRegion
{
public:
    int startLine;
    int endLine;
    bool folded;

    FoldRegion(int start, int end) : startLine(start), endLine(end), folded(false) {}
};

class CodeFoldData : public QTextBlockUserData
{
public:
    QList<FoldRegion> foldRegions;
    void addRegion(int start, int end)
    {
        foldRegions.append(FoldRegion(start, end));
    }
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr,
                            const QString &currentCompilerPath = "",
                            bool autoBrackets = true,
                            bool autoQuotes = true,
                            bool autoIndent = true,
                            int indentSize = 4,
                            bool codeFolding = true,
                            bool lineNumbers = true,
                            bool darkTheme = true,
                            bool enableAI = false,
                            const QString &aiEndpoint = "",
                            const QString &aiKey = "");

    QString getCompilerPath() const;
    bool getAutoBrackets() const;
    bool getAutoQuotes() const;
    bool getAutoIndent() const;
    int getIndentSize() const;
    bool getCodeFolding() const;
    bool getLineNumbers() const;
    bool getDarkTheme() const;
    bool getEnableAI() const;
    QString getAIEndpoint() const;
    QString getAIKey() const;

private slots:
    void browseCompilerPath();
    void toggleAISettings(bool enabled);

private:
    QLineEdit *compilerPathEdit;
    QCheckBox *autoBracketsCheck;
    QCheckBox *autoQuotesCheck;
    QCheckBox *autoIndentCheck;
    QSpinBox *indentSizeSpin;
    QCheckBox *codeFoldingCheck;
    QCheckBox *lineNumbersCheck;
    QCheckBox *darkThemeCheck;
    QCheckBox *enableAICheck;
    QLineEdit *aiEndpointEdit;
    QLineEdit *aiKeyEdit;
    QString findCompilerInPath(const QString &basePath);
    void extractCompiler();

private slots:
    void downloadCompiler();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();
    void onExtractFinished();
    void browseDownloadPath();

private:
    QPushButton *downloadCompilerButton;
    QLineEdit *downloadPathEdit;
    QPushButton *browseDownloadButton;
    QNetworkAccessManager *networkManager;
    QNetworkReply *downloadReply;
    QProgressDialog *progressDialog;
    QString downloadedFilePath;
};

SettingsDialog::SettingsDialog(QWidget *parent, const QString &currentCompilerPath,
                               bool autoBrackets, bool autoQuotes, bool autoIndent,
                               int indentSize, bool codeFolding, bool lineNumbers,
                               bool darkTheme, bool enableAI, const QString &aiEndpoint,
                               const QString &aiKey)
    : QDialog(parent), networkManager(nullptr), downloadReply(nullptr)
{
    setWindowTitle(tr("设置 - Compiler IDE 2.8.3"));
    setModal(true);
    setFixedSize(600, 600);

    QFormLayout *formLayout = new QFormLayout;
    QGroupBox *downloadGroup = new QGroupBox(tr("编译器下载"));
    QFormLayout *downloadLayout = new QFormLayout;
    downloadPathEdit = new QLineEdit(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
    browseDownloadButton = new QPushButton(tr("浏览..."));
    connect(browseDownloadButton, &QPushButton::clicked, this, &SettingsDialog::browseDownloadPath);
    QHBoxLayout *downloadPathLayout = new QHBoxLayout;
    downloadPathLayout->addWidget(downloadPathEdit);
    downloadPathLayout->addWidget(browseDownloadButton);
    downloadCompilerButton = new QPushButton(tr("下载 MinGW 编译器"));
    connect(downloadCompilerButton, &QPushButton::clicked, this, &SettingsDialog::downloadCompiler);
    downloadLayout->addRow(tr("下载路径:"), downloadPathLayout);
    downloadLayout->addRow(downloadCompilerButton);
    downloadGroup->setLayout(downloadLayout);
    compilerPathEdit = new QLineEdit(currentCompilerPath);
    QPushButton *browseButton = new QPushButton(tr("浏览..."));
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::browseCompilerPath);
    QHBoxLayout *compilerLayout = new QHBoxLayout;
    compilerLayout->addWidget(compilerPathEdit);
    compilerLayout->addWidget(browseButton);
    formLayout->addRow(tr("编译器路径:"), compilerLayout);
    QGroupBox *editorGroup = new QGroupBox(tr("编辑器设置"));
    QFormLayout *editorLayout = new QFormLayout;
    autoBracketsCheck = new QCheckBox(tr("启用自动括号补全"));
    autoBracketsCheck->setChecked(autoBrackets);
    autoQuotesCheck = new QCheckBox(tr("启用自动引号补全"));
    autoQuotesCheck->setChecked(autoQuotes);
    autoIndentCheck = new QCheckBox(tr("启用自动缩进"));
    autoIndentCheck->setChecked(autoIndent);
    indentSizeSpin = new QSpinBox;
    indentSizeSpin->setRange(1, 8);
    indentSizeSpin->setValue(indentSize);
    indentSizeSpin->setSuffix(tr(" 个空格"));
    codeFoldingCheck = new QCheckBox(tr("启用代码折叠"));
    codeFoldingCheck->setChecked(codeFolding);
    lineNumbersCheck = new QCheckBox(tr("显示行号"));
    lineNumbersCheck->setChecked(lineNumbers);
    darkThemeCheck = new QCheckBox(tr("使用暗色主题"));
    darkThemeCheck->setChecked(darkTheme);
    editorLayout->addRow(autoBracketsCheck);
    editorLayout->addRow(autoQuotesCheck);
    editorLayout->addRow(autoIndentCheck);
    editorLayout->addRow(tr("缩进大小:"), indentSizeSpin);
    editorLayout->addRow(codeFoldingCheck);
    editorLayout->addRow(lineNumbersCheck);
    editorLayout->addRow(darkThemeCheck);
    editorGroup->setLayout(editorLayout);
    QGroupBox *aiGroup = new QGroupBox(tr("AI辅助编程（暂不开放）"));
    QFormLayout *aiLayout = new QFormLayout;
    enableAICheck = new QCheckBox(tr("启用AI代码建议"));
    enableAICheck->setChecked(enableAI);
    connect(enableAICheck, &QCheckBox::toggled, this, &SettingsDialog::toggleAISettings);
    aiEndpointEdit = new QLineEdit(aiEndpoint);
    aiEndpointEdit->setPlaceholderText(tr("https://api.example.com/v1/completions"));
    aiKeyEdit = new QLineEdit(aiKey);
    aiKeyEdit->setPlaceholderText(tr("输入API密钥"));
    aiKeyEdit->setEchoMode(QLineEdit::Password);
    aiLayout->addRow(enableAICheck);
    aiLayout->addRow(tr("AI端点:"), aiEndpointEdit);
    aiLayout->addRow(tr("API密钥:"), aiKeyEdit);
    aiGroup->setLayout(aiLayout);
    aiGroup->setEnabled(enableAI);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(downloadGroup);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(editorGroup);
    mainLayout->addWidget(aiGroup);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    networkManager = new QNetworkAccessManager(this);
}
void SettingsDialog::downloadCompiler()
{
    downloadCompilerButton->setEnabled(false);
    progressDialog = new QProgressDialog(tr("正在下载 MinGW 编译器..."), tr("取消"), 0, 100, this);
    progressDialog->setWindowTitle(tr("下载编译器"));
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(0);
    progressDialog->setValue(0);
    connect(progressDialog, &QProgressDialog::canceled, this, [this]()
    {
        if (downloadReply)
        {
            downloadReply->abort();
        }
    });
    QUrl downloadUrl("https://downloads.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win64/Personal%20Builds/mingw-builds/8.1.0/threads-posix/seh/x86_64-8.1.0-release-posix-seh-rt_v6-rev0.7z");
    QNetworkRequest request(downloadUrl);
    request.setRawHeader("User-Agent", "CompilerIDE/2.8.3");
    downloadReply = networkManager->get(request);
    connect(downloadReply, &QNetworkReply::downloadProgress,
            this, &SettingsDialog::onDownloadProgress);
    connect(downloadReply, &QNetworkReply::finished,
            this, &SettingsDialog::onDownloadFinished);
}
void SettingsDialog::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0)
    {
        int progress = static_cast<int>((bytesReceived * 100) / bytesTotal);
        progressDialog->setValue(progress);
        QString sizeReceived = QString::number(bytesReceived / (1024.0 * 1024.0), 'f', 2);
        QString sizeTotal = QString::number(bytesTotal / (1024.0 * 1024.0), 'f', 2);
        progressDialog->setLabelText(tr("正在下载 MinGW 编译器...\n%1 MB / %2 MB")
                                         .arg(sizeReceived).arg(sizeTotal));
    }
}

void SettingsDialog::onDownloadFinished()
{
    if (downloadReply->error() != QNetworkReply::NoError)
    {
        if (downloadReply->error() != QNetworkReply::OperationCanceledError)
        {
            QMessageBox::critical(this, tr("下载失败"),
                                  tr("下载编译器失败: %1").arg(downloadReply->errorString()));
        }
        progressDialog->close();
        downloadCompilerButton->setEnabled(true);
        downloadReply->deleteLater();
        downloadReply = nullptr;
        return;
    }
    QString downloadDir = downloadPathEdit->text();
    QDir dir(downloadDir);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }
    downloadedFilePath = downloadDir + "/mingw.7z";
    QFile file(downloadedFilePath);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(downloadReply->readAll());
        file.close();
        progressDialog->setLabelText(tr("下载完成，正在解压..."));
        progressDialog->setValue(100);
        extractCompiler();
    }
    else
    {
        QMessageBox::critical(this, tr("保存失败"),
                              tr("无法保存下载的文件: %1").arg(file.errorString()));
        progressDialog->close();
        downloadCompilerButton->setEnabled(true);
    }
    downloadReply->deleteLater();
    downloadReply = nullptr;
}

void SettingsDialog::extractCompiler()
{
    QString extractPath = QFileDialog::getExistingDirectory(this,
                                                            tr("选择解压路径"), downloadPathEdit->text());
    if (extractPath.isEmpty())
    {
        progressDialog->close();
        downloadCompilerButton->setEnabled(true);
        return;
    }
    progressDialog->setLabelText(tr("正在解压编译器..."));
    progressDialog->setRange(0, 0);
    QProcess *extractProcess = new QProcess(this);
    connect(extractProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &SettingsDialog::onExtractFinished);

#ifdef Q_OS_WIN
    QString program = "tar";
    QStringList arguments;
    arguments << "xf" << downloadedFilePath << "-C" << extractPath;
    extractProcess->start("tar", arguments);
    if (!extractProcess->waitForStarted(1000))
    {
        program = "7z";
        arguments = QStringList() << "x" << downloadedFilePath << "-o" + extractPath << "-y";
        extractProcess->start("7z", arguments);
    }
#else
    QString program = "tar";
    QStringList arguments;
    arguments << "xf" << downloadedFilePath << "-C" << extractPath;
    extractProcess->start(program, arguments);
#endif
}
void SettingsDialog::onExtractFinished()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    if (!process) return;

    if (process->exitCode() == 0)
    {
        progressDialog->close();
        QString extractPath = QFileInfo(downloadedFilePath).absolutePath();
        QString compilerPath = findCompilerInPath(extractPath);

        if (!compilerPath.isEmpty())
        {
            compilerPathEdit->setText(compilerPath);
            QMessageBox::information(this, tr("下载完成"),
                                     tr("MinGW 编译器下载并解压完成！\n编译器路径已自动设置。"));
        }
        else
        {
            QMessageBox::information(this, tr("下载完成"),
                                     tr("MinGW 编译器下载并解压完成！\n请手动选择编译器路径。"));
        }
    }
    else
    {
        QMessageBox::critical(this, tr("解压失败"),
                              tr("解压编译器失败。错误信息:\n%1").arg(QString::fromLocal8Bit(process->readAllStandardError())));
    }

    process->deleteLater();
    downloadCompilerButton->setEnabled(true);
    QFile::remove(downloadedFilePath);
}
QString SettingsDialog::findCompilerInPath(const QString &basePath)
{
    QDir dir(basePath);
    if (!dir.exists()) return QString();
    QDirIterator it(basePath, QStringList() << "g++.exe" << "g++",
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString compilerPath = it.next();
        if (compilerPath.contains("mingw", Qt::CaseInsensitive) ||
            compilerPath.contains("gcc", Qt::CaseInsensitive))
        {
            return QDir::toNativeSeparators(compilerPath);
        }
    }
    return QString();
}
void SettingsDialog::browseDownloadPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("选择下载路径"),
                                                     downloadPathEdit->text());
    if (!path.isEmpty())
    {
        downloadPathEdit->setText(path);
    }
}
QString SettingsDialog::getCompilerPath() const
{
    return compilerPathEdit->text();
}
bool SettingsDialog::getAutoBrackets() const
{
    return autoBracketsCheck->isChecked();
}
bool SettingsDialog::getAutoQuotes() const
{
    return autoQuotesCheck->isChecked();
}
bool SettingsDialog::getAutoIndent() const
{
    return autoIndentCheck->isChecked();
}
int SettingsDialog::getIndentSize() const
{
    return indentSizeSpin->value();
}
bool SettingsDialog::getCodeFolding() const
{
    return codeFoldingCheck->isChecked();
}
bool SettingsDialog::getLineNumbers() const
{
    return lineNumbersCheck->isChecked();
}
bool SettingsDialog::getDarkTheme() const
{
    return darkThemeCheck->isChecked();
}
bool SettingsDialog::getEnableAI() const
{
    return enableAICheck->isChecked();
}
QString SettingsDialog::getAIEndpoint() const
{
    return aiEndpointEdit->text();
}
QString SettingsDialog::getAIKey() const
{
    return aiKeyEdit->text();
}
void SettingsDialog::browseCompilerPath()
{
    QString path = QFileDialog::getOpenFileName(this, tr("选择编译器"),
                                                QDir::homePath(),
#ifdef Q_OS_WIN
                                                tr("可执行文件 (*.exe);;所有文件 (*)"));
#else
                                                tr("所有文件 (*)"));
#endif
    if (!path.isEmpty())
    {
        compilerPathEdit->setText(path);
    }
}
void SettingsDialog::toggleAISettings(bool enabled)
{
    for (int i = 0; i < layout()->count(); ++i)
    {
        QLayoutItem *item = layout()->itemAt(i);
        if (QGroupBox *groupBox = qobject_cast<QGroupBox*>(item->widget()))
        {
            if (groupBox->title() == tr("AI辅助编程"))
            {
                groupBox->setEnabled(enabled);
                break;
            }
        }
    }
}

class SplashScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();
    void showWithAnimation();
    void hideWithAnimation();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onHideAnimationFinished();
signals:
    void hidden();
private:
    void setupUI();
    void setupAnimations();
    QLabel *titleLabel;
    QLabel *versionLabel;
    QLabel *loadingLabel;
    QProgressBar *progressBar;
    QVBoxLayout *mainLayout;
    QPropertyAnimation *showAnimation;
    QPropertyAnimation *hideAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    QTimer *progressTimer;
    int progressValue;
};
SplashScreen::SplashScreen(QWidget *parent)
    : QWidget(parent), progressValue(0)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::SplashScreen);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(600, 400);

    setupUI();
    setupAnimations();
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.center() - rect().center());
}
SplashScreen::~SplashScreen()
{
    if (progressTimer)
    {
        progressTimer->stop();
        delete progressTimer;
    }
}

void SplashScreen::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    titleLabel = new QLabel("Compiler IDE", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 32, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #2E86AB; background: transparent;");
    versionLabel = new QLabel("Version 2.8.6", this);
    versionLabel->setAlignment(Qt::AlignCenter);
    QFont versionFont("Arial", 14, QFont::Normal);
    versionLabel->setFont(versionFont);
    versionLabel->setStyleSheet("color: #A23B72; background: transparent;");
    loadingLabel = new QLabel("正在启动...", this);
    loadingLabel->setAlignment(Qt::AlignCenter);
    QFont loadingFont("Arial", 10, QFont::Normal);
    loadingLabel->setFont(loadingFont);
    loadingLabel->setStyleSheet("color: #F18F01; background: transparent;");
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(false);
    progressBar->setFixedHeight(6);
    progressBar->setStyleSheet(
        "QProgressBar {"
        "    border: none;"
        "    background: rgba(255, 255, 255, 30);"
        "    border-radius: 3px;"
        "}"
        "QProgressBar::chunk {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "    stop:0 #2E86AB, stop:0.5 #A23B72, stop:1 #F18F01);"
        "    border-radius: 3px;"
        "}"
        );
    mainLayout->addStretch();
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(versionLabel);
    mainLayout->addStretch();
    mainLayout->addWidget(loadingLabel);
    mainLayout->addWidget(progressBar);
    opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.0);
    setGraphicsEffect(opacityEffect);
}
void SplashScreen::setupAnimations()
{
    showAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    showAnimation->setDuration(800);
    showAnimation->setStartValue(0.0);
    showAnimation->setEndValue(1.0);
    showAnimation->setEasingCurve(QEasingCurve::OutCubic);
    hideAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    hideAnimation->setDuration(600);
    hideAnimation->setStartValue(1.0);
    hideAnimation->setEndValue(0.0);
    hideAnimation->setEasingCurve(QEasingCurve::InCubic);
    connect(hideAnimation, &QPropertyAnimation::finished, this, &SplashScreen::onHideAnimationFinished);
    progressTimer = new QTimer(this);
    connect(progressTimer, &QTimer::timeout, this, [this]()
    {
        progressValue += 2;
        progressBar->setValue(progressValue);
        if (progressValue < 25)
        {
            loadingLabel->setText("正在初始化核心模块...");
        }
        else if (progressValue < 50)
        {
            loadingLabel->setText("正在加载编辑器组件...");
        }
        else if (progressValue < 75)
        {
            loadingLabel->setText("正在配置编译器设置...");
        }
        else
        {
            loadingLabel->setText("正在准备用户界面...");
        }
        if (progressValue >= 100)
        {
            progressTimer->stop();
            hideWithAnimation();
        }
    });
}
void SplashScreen::showWithAnimation()
{
    show();
    showAnimation->start();
    progressTimer->start(30);
}
void SplashScreen::hideWithAnimation()
{
    hideAnimation->start();
}
void SplashScreen::onHideAnimationFinished()
{
    hide();
    emit hidden();
}
void SplashScreen::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QLinearGradient gradient(0, 0, width(), height());
    gradient.setColorAt(0, QColor(30, 30, 40));
    gradient.setColorAt(0.5, QColor(40, 40, 55));
    gradient.setColorAt(1, QColor(25, 25, 35));
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);
    QPen borderPen(QColor(100, 100, 150, 100));
    borderPen.setWidth(2);
    painter.setPen(borderPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 20, 20);
    painter.setPen(QPen(QColor(46, 134, 171, 50), 3));
    painter.drawLine(50, height() - 50, width() - 50, height() - 50);
}

class WelcomeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WelcomeDialog(QWidget *parent = nullptr);
    ~WelcomeDialog();
    QString getSelectedFile() const { return selectedFile; }

private slots:
    void onNewProjectClicked();
    void onOpenProjectClicked();
    void onOpenFileClicked();
    void onRecentFileClicked(QListWidgetItem *item);
    void onThemeToggled(bool dark);
    void clearRecentFiles();

private:
    void setupUI();
    void setupConnections();
    void loadRecentFiles();
    void saveRecentFiles();
    void addToRecentFiles(const QString &filePath);
    QListWidget *recentList;
    QPushButton *newProjectBtn;
    QPushButton *openProjectBtn;
    QPushButton *openFileBtn;
    QPushButton *clearRecentBtn;
    QCheckBox *darkThemeCheck;
    QStringList recentFiles;
    QString selectedFile;
};

WelcomeDialog::WelcomeDialog(QWidget *parent)
    : QDialog(parent), selectedFile("")
{
    setWindowTitle("欢迎使用 Compiler IDE 2.8.6");
    setFixedSize(800, 600);
    setModal(true);
    setupUI();
    setupConnections();
    loadRecentFiles();
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    move(screenGeometry.center() - rect().center());
}
WelcomeDialog::~WelcomeDialog()
{
    saveRecentFiles();
}
void WelcomeDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    QWidget *headerWidget = new QWidget(this);
    headerWidget->setFixedHeight(120);
    headerWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
        "stop:0 #2E86AB, stop:0.3 #A23B72, stop:0.6 #F18F01, stop:1 #C73E1D);"
        "border-top-left-radius: 10px;"
        "border-top-right-radius: 10px;"
        );
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(40, 0, 40, 0);
    QLabel *titleLabel = new QLabel("Compiler IDE 2.8.6", headerWidget);
    titleLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold; background: transparent;");
    QLabel *subtitleLabel = new QLabel("现代化 C++ 开发环境", headerWidget);
    subtitleLabel->setStyleSheet("color: rgba(255,255,255,0.8); font-size: 14px; background: transparent;");
    QVBoxLayout *textLayout = new QVBoxLayout();
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(subtitleLabel);
    textLayout->setSpacing(5);
    headerLayout->addLayout(textLayout);
    headerLayout->addStretch();
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setStyleSheet("background: #2D3047; border-bottom-left-radius: 10px; border-bottom-right-radius: 10px;");
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 30, 40, 30);
    contentLayout->setSpacing(40);
    QWidget *quickActionsWidget = new QWidget(contentWidget);
    quickActionsWidget->setFixedWidth(300);
    QVBoxLayout *actionsLayout = new QVBoxLayout(quickActionsWidget);
    actionsLayout->setSpacing(15);
    QLabel *actionsTitle = new QLabel("快速开始", quickActionsWidget);
    actionsTitle->setStyleSheet("color: #E0E0E0; font-size: 20px; font-weight: bold; margin-bottom: 10px;");
    newProjectBtn = new QPushButton("新建项目", quickActionsWidget);
    newProjectBtn->setFixedHeight(50);
    newProjectBtn->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #2E86AB, stop:1 #A23B72);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #3B9BC8, stop:1 #B84F8A);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1E5A7A, stop:1 #7C2C5A);"
        "}"
        );
    openProjectBtn = new QPushButton("打开项目", quickActionsWidget);
    openProjectBtn->setFixedHeight(50);
    openProjectBtn->setStyleSheet(
        "QPushButton {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #F18F01, stop:1 #C73E1D);"
        "    color: white;"
        "    border: none;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFA726, stop:1 #E64A19);"
        "}"
        "QPushButton:pressed {"
        "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #B26A00, stop:1 #9C2700);"
        "}"
        );
  
    openFileBtn = new QPushButton("打开文件", quickActionsWidget);
    openFileBtn->setFixedHeight(50);
    openFileBtn->setStyleSheet(
        "QPushButton {"
        "    background: #404764;"
        "    color: white;"
        "    border: 2px solid #5A5F7A;"
        "    border-radius: 8px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background: #4A5070;"
        "    border-color: #6B7190;"
        "}"
        "QPushButton:pressed {"
        "    background: #363A52;"
        "}"
        );
    QWidget *themeWidget = new QWidget(quickActionsWidget);
    QHBoxLayout *themeLayout = new QHBoxLayout(themeWidget);
    themeLayout->setContentsMargins(0, 0, 0, 0);
    QLabel *themeLabel = new QLabel("界面主题:", themeWidget);
    themeLabel->setStyleSheet("color: #B0B0B0;");
    darkThemeCheck = new QCheckBox("暗色主题", themeWidget);
    darkThemeCheck->setChecked(true);
    darkThemeCheck->setStyleSheet(
        "QCheckBox { color: #B0B0B0; }"
        "QCheckBox::indicator { width: 16px; height: 16px; }"
        "QCheckBox::indicator:unchecked { background: #404764; border: 1px solid #5A5F7A; }"
        "QCheckBox::indicator:checked { background: #2E86AB; border: 1px solid #3B9BC8; }"
        );
    themeLayout->addWidget(themeLabel);
    themeLayout->addWidget(darkThemeCheck);
    themeLayout->addStretch();
    actionsLayout->addWidget(actionsTitle);
    actionsLayout->addWidget(newProjectBtn);
    actionsLayout->addWidget(openProjectBtn);
    actionsLayout->addWidget(openFileBtn);
    actionsLayout->addStretch();
    actionsLayout->addWidget(themeWidget);
    QWidget *recentWidget = new QWidget(contentWidget);
    QVBoxLayout *recentLayout = new QVBoxLayout(recentWidget);
    recentLayout->setSpacing(15);
    QHBoxLayout *recentHeaderLayout = new QHBoxLayout();
    QLabel *recentTitle = new QLabel("最近文件", recentWidget);
    recentTitle->setStyleSheet("color: #E0E0E0; font-size: 20px; font-weight: bold;");
    clearRecentBtn = new QPushButton("清空列表", recentWidget);
    clearRecentBtn->setFixedSize(80, 25);
    clearRecentBtn->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    color: #F18F01;"
        "    border: 1px solid #F18F01;"
        "    border-radius: 4px;"
        "    font-size: 11px;"
        "}"
        "QPushButton:hover {"
        "    background: rgba(241, 143, 1, 0.2);"
        "}"
        "QPushButton:pressed {"
        "    background: rgba(241, 143, 1, 0.3);"
        "}"
        );

    recentHeaderLayout->addWidget(recentTitle);
    recentHeaderLayout->addStretch();
    recentHeaderLayout->addWidget(clearRecentBtn);
    recentList = new QListWidget(recentWidget);
    recentList->setStyleSheet(
        "QListWidget {"
        "    background: #404764;"
        "    border: 1px solid #5A5F7A;"
        "    border-radius: 8px;"
        "    color: #E0E0E0;"
        "    font-size: 12px;"
        "    outline: none;"
        "}"
        "QListWidget::item {"
        "    border-bottom: 1px solid #5A5F7A;"
        "    padding: 12px;"
        "}"
        "QListWidget::item:selected {"
        "    background: #2E86AB;"
        "    color: white;"
        "}"
        "QListWidget::item:hover {"
        "    background: #4A5070;"
        "}"
        "QListWidget::item:last {"
        "    border-bottom: none;"
        "}"
        );
    recentLayout->addLayout(recentHeaderLayout);
    recentLayout->addWidget(recentList);
    contentLayout->addWidget(quickActionsWidget);
    contentLayout->addWidget(recentWidget);
    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(contentWidget);
}
void WelcomeDialog::setupConnections()
{
    connect(newProjectBtn, &QPushButton::clicked, this, &WelcomeDialog::onNewProjectClicked);
    connect(openProjectBtn, &QPushButton::clicked, this, &WelcomeDialog::onOpenProjectClicked);
    connect(openFileBtn, &QPushButton::clicked, this, &WelcomeDialog::onOpenFileClicked);
    connect(recentList, &QListWidget::itemClicked, this, &WelcomeDialog::onRecentFileClicked);
    connect(darkThemeCheck, &QCheckBox::toggled, this, &WelcomeDialog::onThemeToggled);
    connect(clearRecentBtn, &QPushButton::clicked, this, &WelcomeDialog::clearRecentFiles);
}
void WelcomeDialog::loadRecentFiles()
{
    QSettings settings("CompilerIDE", "Compiler IDE 2.8.6");
    recentFiles = settings.value("recentFiles").toStringList();
    recentList->clear();
    for (const QString &filePath : recentFiles)
    {
        if (QFile::exists(filePath))
        {
            QFileInfo fileInfo(filePath);
            QString displayText = QString("%1\n%2")
                                      .arg(fileInfo.fileName())
                                      .arg(fileInfo.absolutePath());

            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, filePath);
            if (fileInfo.suffix().toLower() == "cpp" || fileInfo.suffix().toLower() == "cc")
            {
                item->setForeground(QColor("#2E86AB"));
            } 
            else if (fileInfo.suffix().toLower() == "h" || fileInfo.suffix().toLower() == "hpp")
            {
                item->setForeground(QColor("#A23B72"));
            }
            else
            {
                item->setForeground(QColor("#F18F01"));
            }
            recentList->addItem(item);
        }
    }
    if (recentList->count() == 0)
    {
        QListWidgetItem *item = new QListWidgetItem("暂无最近打开的文件");
        item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
        item->setForeground(QColor("#666666"));
        recentList->addItem(item);
    }
}
void WelcomeDialog::saveRecentFiles()
{
    QSettings settings("CompilerIDE", "Compiler IDE 2.8.6");
    settings.setValue("recentFiles", recentFiles);
}
void WelcomeDialog::addToRecentFiles(const QString &filePath)
{
    if (filePath.isEmpty() || !QFile::exists(filePath))
    {
        return;
    }
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 10)
    {
        recentFiles.removeLast();
    }
    saveRecentFiles();
}
void WelcomeDialog::onNewProjectClicked()
{
    selectedFile = "";
    accept();
}
void WelcomeDialog::onOpenProjectClicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择项目文件夹", "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        QDir projectDir(dir);
        QStringList cppFiles = projectDir.entryList(QStringList() << "*.cpp" << "*.cc" << "*.cxx", QDir::Files);

        if (!cppFiles.isEmpty())
        {
            selectedFile = projectDir.absoluteFilePath(cppFiles.first());
            addToRecentFiles(selectedFile);
        }
        else
        {
            selectedFile = projectDir.absoluteFilePath("main.cpp");
        }
        accept();
    }
}
void WelcomeDialog::onOpenFileClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "打开文件", "",
                                                    "C++文件 (*.cpp *.cc *.cxx *.c++ *.h *.hpp *.hh *.hxx *.h++);;所有文件 (*)");
    if (!fileName.isEmpty())
    {
        selectedFile = fileName;
        addToRecentFiles(selectedFile);
        accept();
    }
}
void WelcomeDialog::onRecentFileClicked(QListWidgetItem *item)
{
    if (item && (item->flags() & Qt::ItemIsEnabled))
    {
        QString filePath = item->data(Qt::UserRole).toString();
        if (QFile::exists(filePath))
        {
            selectedFile = filePath;
            recentFiles.removeAll(filePath);
            recentFiles.prepend(filePath);
            accept();
        }
        else
        {
            QMessageBox::warning(this, "文件不存在", "所选文件不存在，已从最近文件列表中移除。");
            recentFiles.removeAll(filePath);
            delete recentList->takeItem(recentList->row(item)); // 安全移除项目
        }
    }
}
void WelcomeDialog::onThemeToggled(bool dark)
{
    QSettings settings("CompilerIDE", "Compiler IDE 2.8.6");
    settings.setValue("darkTheme", dark);
    if (dark)
    {
        setStyleSheet(
            "WelcomeDialog { background: #2D3047; }"
            "QGroupBox { color: #E0E0E0; border: 1px solid #5A5F7A; }"
            "QLabel { color: #E0E0E0; }"
            "QPushButton { background: #404764; color: white; border: 1px solid #5A5F7A; }"
            "QListWidget { background: #404764; color: #E0E0E0; }"
            "QCheckBox { color: #E0E0E0; }"
            );
    }
    else
    {
        setStyleSheet(
            "WelcomeDialog { background: #F5F5F5; }"
            "QGroupBox { color: #333333; border: 1px solid #CCCCCC; }"
            "QLabel { color: #333333; }"
            "QPushButton { background: #E0E0E0; color: #333333; border: 1px solid #CCCCCC; }"
            "QListWidget { background: white; color: #333333; }"
            "QCheckBox { color: #333333; }"
            );
    }
}
void WelcomeDialog::clearRecentFiles()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认清空", "确定要清空最近文件列表吗？",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        recentFiles.clear();
        saveRecentFiles();
        loadRecentFiles();
    }
}

class CppHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit CppHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat preprocessorFormat;
    QTextCharFormat numberFormat;
    QTextCharFormat typeFormat;
    QTextCharFormat macroFormat;
};

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    keywordFormat.setForeground(QColor(86, 156, 214));
    keywordFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(184, 115, 212));
    classFormat.setFontWeight(QFont::Bold);
    singleLineCommentFormat.setForeground(QColor(106, 153, 85));
    multiLineCommentFormat.setForeground(QColor(106, 153, 85));
    quotationFormat.setForeground(QColor(206, 145, 120));
    functionFormat.setForeground(QColor(78, 201, 176));
    preprocessorFormat.setForeground(QColor(86, 156, 214));
    numberFormat.setForeground(QColor(181, 206, 168));
    typeFormat.setForeground(QColor(78, 201, 176));
    typeFormat.setFontWeight(QFont::Bold);
    macroFormat.setForeground(QColor(200, 100, 200));
    QStringList keywordPatterns;
    keywordPatterns << "\\bchar\\b" << "\\bclass\\b" << "\\bconst\\b"
                    << "\\bdouble\\b" << "\\benum\\b" << "\\bexplicit\\b"
                    << "\\bfriend\\b" << "\\binline\\b" << "\\bint\\b"
                    << "\\blong\\b" << "\\bnamespace\\b" << "\\boperator\\b"
                    << "\\bprivate\\b" << "\\bprotected\\b" << "\\bpublic\\b"
                    << "\\bshort\\b" << "\\bsignals\\b" << "\\bsigned\\b"
                    << "\\bslots\\b" << "\\bstatic\\b" << "\\bstruct\\b"
                    << "\\btemplate\\b" << "\\btypedef\\b" << "\\btypename\\b"
                    << "\\bunion\\b" << "\\bunsigned\\b" << "\\bvirtual\\b"
                    << "\\bvoid\\b" << "\\bvolatile\\b" << "\\bbool\\b"
                    << "\\busing\\b" << "\\breturn\\b" << "\\bif\\b"
                    << "\\belse\\b" << "\\bfor\\b" << "\\bwhile\\b"
                    << "\\bdo\\b" << "\\bswitch\\b" << "\\bcase\\b"
                    << "\\bbreak\\b" << "\\bcontinue\\b" << "\\bgoto\\b"
                    << "\\btry\\b" << "\\bcatch\\b" << "\\bthrow\\b"
                    << "\\bnew\\b" << "\\bdelete\\b" << "\\bthis\\b"
                    << "\\band\\b" << "\\bnot\\b" << "\\bor\\b"
                    << "\\bxor\\b" << "\\band_eq\\b" << "\\bor_eq\\b"
                    << "\\bxor_eq\\b" << "\\bnot_eq\\b" << "\\bbitand\\b"
                    << "\\bbitor\\b" << "\\bcompl\\b" << "\\basm\\b"
                    << "\\bexport\\b" << "\\bextern\\b" << "\\bfalse\\b"
                    << "\\btrue\\b" << "\\bmutable\\b" << "\\btypename\\b"
                    << "\\btypeid\\b" << "\\bwchar_t\\b" << "\\bstatic_cast\\b"
                    << "\\bconst_cast\\b" << "\\bdynamic_cast\\b"
                    << "\\breinterpret_cast\\b" << "\\bnullptr\\b"
                    << "\\bdecltype\\b" << "\\bthread_local\\b"
                    << "\\balignas\\b" << "\\balignof\\b" << "\\bstatic_assert\\b"
                    << "\\bnoexcept\\b" << "\\boverride\\b" << "\\bfinal\\b"
                    << "\\bconcept\\b" << "\\brequires\\b" << "\\bconsteval\\b"
                    << "\\bconstinit\\b" << "\\bconstexpr\\b" << "\\bco_await\\b"
                    << "\\bco_yield\\b" << "\\bco_return\\b" << "\\bimport\\b"
                    << "\\bmodule\\b" << "\\breflexpr\\b" << "\\bauto\\b"
                    << "\\bconcept\\b" << "\\brequires\\b" << "\\bconsteval\\b"
                    << "\\bconstinit\\b" << "\\bconstexpr\\b" << "\\bco_await\\b"
                    << "\\bco_yield\\b" << "\\bco_return\\b" << "\\bimport\\b"
                    << "\\bmodule\\b" << "\\breflexpr\\b" << "\\bauto\\b";
    for (const QString &pattern : keywordPatterns)
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    HighlightingRule classRule;
    classRule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    classRule.format = classFormat;
    highlightingRules.append(classRule);
    HighlightingRule functionRule;
    functionRule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    functionRule.format = functionFormat;
    highlightingRules.append(functionRule);
    HighlightingRule preprocessorRule;
    preprocessorRule.pattern = QRegularExpression("#[^\\n]*");
    preprocessorRule.format = preprocessorFormat;
    highlightingRules.append(preprocessorRule);
    HighlightingRule quotationRule;
    quotationRule.pattern = QRegularExpression("\".*?\"");
    quotationRule.format = quotationFormat;
    highlightingRules.append(quotationRule);
    HighlightingRule charRule;
    charRule.pattern = QRegularExpression("'.*?'");
    charRule.format = quotationFormat;
    highlightingRules.append(charRule);
    HighlightingRule singleLineCommentRule;
    singleLineCommentRule.pattern = QRegularExpression("//[^\\n]*");
    singleLineCommentRule.format = singleLineCommentFormat;
    highlightingRules.append(singleLineCommentRule);
    HighlightingRule numberRule;
    numberRule.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?\\b");
    numberRule.format = numberFormat;
    highlightingRules.append(numberRule);
    HighlightingRule typeRule;
    typeRule.pattern = QRegularExpression("\\b(int|float|double|char|void|bool|short|long|unsigned|signed)\\b");
    typeRule.format = typeFormat;
    highlightingRules.append(typeRule);
    HighlightingRule macroRule;
    macroRule.pattern = QRegularExpression("\\b[A-Z_][A-Z0-9_]*\\b");
    macroRule.format = macroFormat;
    highlightingRules.append(macroRule);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}
void CppHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : highlightingRules)
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);
    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

class CodeCompleter;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void foldAreaPaintEvent(QPaintEvent *event);
    int foldAreaWidth();
    void setAutoBracketsEnabled(bool enabled);
    void setAutoQuotesEnabled(bool enabled);
    void setAutoIndentEnabled(bool enabled);
    void setIndentSize(int size);
    void setCodeFoldingEnabled(bool enabled);
    void setLineNumbersEnabled(bool enabled);
    void setDarkThemeEnabled(bool enabled);
    void toggleFold(int line);
    void updateFoldRegions();
    bool isFolded(int line) const;
    void setCompleter(CodeCompleter *comp);
    void setCompletionEnabled(bool enabled) { completionEnabled = enabled; }
    bool isCompletionEnabled() const { return completionEnabled; }

private:
    CodeCompleter *completer;
    QListView *completionList;
    QWidget *completionPopup;
    QString completionPrefix;
    bool completionEnabled;
    bool ignoreNextKeyPress;
    void showCompletion(const QStringList &completions);
    void insertCompletion(const QString &completion);
    void hideCompletion();
    QString getWordUnderCursor() const;
    bool eventFilter(QObject *obj, QEvent *event) override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected:
    void focusInEvent(QFocusEvent *event) override
    {
        QPlainTextEdit::focusInEvent(event);
        setFocus();
    }

    void focusOutEvent(QFocusEvent *event) override
    {
        if (completionPopup && completionPopup->isVisible())
        {
            setFocus();
        }
        else
        {
            QPlainTextEdit::focusOutEvent(event);
        }
    }


private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);
    void updateFoldArea(const QRect &rect, int dy);

private:
    // 行号区域类
    class LineNumberArea : public QWidget
    {
    public:
        LineNumberArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

        QSize sizeHint() const override {
            return QSize(codeEditor->lineNumberAreaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent *event) override {
            codeEditor->lineNumberAreaPaintEvent(event);
        }

    private:
        CodeEditor *codeEditor;
    };

    // 折叠区域类
    class FoldArea : public QWidget
    {
    public:
        FoldArea(CodeEditor *editor) : QWidget(editor), codeEditor(editor) {}

        QSize sizeHint() const override {
            return QSize(codeEditor->foldAreaWidth(), 0);
        }

    protected:
        void paintEvent(QPaintEvent *event) override {
            codeEditor->foldAreaPaintEvent(event);
        }

    private:
        CodeEditor *codeEditor;
    };

    QWidget *lineNumberArea;
    QWidget *foldArea;
    bool autoBrackets;
    bool autoQuotes;
    bool autoIndent;
    int indentSize;
    bool codeFolding;
    bool lineNumbers;
    bool darkTheme;

    QList<FoldRegion> foldRegions;

    void handleAutoBrackets(QKeyEvent *event);
    void handleAutoQuotes(QKeyEvent *event);
    void handleAutoIndent();
    void updateExtraSelections();
    void drawFoldMarker(QPainter *painter, const QRect &rect, bool folded);
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent), completer(nullptr), completionEnabled(true),
    autoBrackets(true), autoQuotes(true), autoIndent(true), indentSize(4),
    codeFolding(true), lineNumbers(true), darkTheme(true), ignoreNextKeyPress(false)
{
    // 创建行号区域和折叠区域
    lineNumberArea = new LineNumberArea(this);
    foldArea = new FoldArea(this);

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateFoldArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    // 设置等宽字体
    QFont font;
    font.setFamily("Consolas");
    font.setStyleHint(QFont::TypeWriter);
    font.setFixedPitch(true);
    font.setPointSize(11);
    setFont(font);

    // 设置Tab宽度
    setTabStopDistance(indentSize * fontMetrics().horizontalAdvance(' '));

    // 设置现代颜色方案
    QPalette p = palette();
    p.setColor(QPalette::Base, QColor(30, 30, 30));
    p.setColor(QPalette::Text, QColor(220, 220, 220));
    setPalette(p);

    // 确保行号区域和折叠区域可见
    lineNumberArea->setVisible(lineNumbers);
    foldArea->setVisible(codeFolding);

    // 初始化自动完成 - 使用无模式、无焦点的窗口
    completionPopup = new QWidget(nullptr, Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    completionPopup->setAttribute(Qt::WA_ShowWithoutActivating);
    completionPopup->setFocusPolicy(Qt::NoFocus);
    completionPopup->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(completionPopup);
    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(0);

    completionList = new QListView(completionPopup);
    completionList->setModel(new QStringListModel(completionPopup));
    completionList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    completionList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    completionList->setFocusPolicy(Qt::NoFocus);
    completionList->setSelectionMode(QAbstractItemView::SingleSelection);
    layout->addWidget(completionList);

    completionPopup->setFixedSize(200, 150);

    // 安装事件过滤器到自动完成列表
    completionList->installEventFilter(this);

    connect(completionList, &QListView::clicked, this, [this](const QModelIndex &index) {
        if (index.isValid()) {
            insertCompletion(index.data().toString());
        }
    });
}

void CodeEditor::setAutoBracketsEnabled(bool enabled)
{
    autoBrackets = enabled;
}

void CodeEditor::setAutoQuotesEnabled(bool enabled)
{
    autoQuotes = enabled;
}

void CodeEditor::setAutoIndentEnabled(bool enabled)
{
    autoIndent = enabled;
}

void CodeEditor::setIndentSize(int size)
{
    indentSize = size;
    setTabStopDistance(indentSize * fontMetrics().horizontalAdvance(' '));
}

bool CodeEditor::eventFilter(QObject *obj, QEvent *event)
{
    // 处理自动完成列表的键盘事件
    if (obj == completionList && completionPopup->isVisible()) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                QModelIndex index = completionList->currentIndex();
                if (index.isValid()) {
                    insertCompletion(index.data().toString());
                }
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Escape) {
                hideCompletion();
                return true;
            }
            else if (keyEvent->key() == Qt::Key_Up || keyEvent->key() == Qt::Key_Down) {
                // 让列表自己处理导航
                return false;
            }
            else if (keyEvent->key() == Qt::Key_Tab) {
                // Tab键选择当前项
                QModelIndex index = completionList->currentIndex();
                if (index.isValid()) {
                    insertCompletion(index.data().toString());
                }
                return true;
            }
            else {
                // 其他按键：隐藏自动完成，让编辑器处理
                hideCompletion();
                setFocus(); // 确保焦点回到编辑器

                // 直接调用编辑器的keyPressEvent，而不是转发事件
                // 这样可以避免复制QKeyEvent的问题
                QKeyEvent newEvent(keyEvent->type(), keyEvent->key(), keyEvent->modifiers(),
                                   keyEvent->text(), keyEvent->isAutoRepeat(), keyEvent->count());
                QPlainTextEdit::keyPressEvent(&newEvent);
                return true;
            }
        }
    }

    return QPlainTextEdit::eventFilter(obj, event);
}

void CodeEditor::setCodeFoldingEnabled(bool enabled)
{
    codeFolding = enabled;
    foldArea->setVisible(enabled);
    if (enabled) {
        updateFoldRegions();
    }
    updateLineNumberAreaWidth(0);
    update();
}

void CodeEditor::setLineNumbersEnabled(bool enabled)
{
    lineNumbers = enabled;
    lineNumberArea->setVisible(enabled);
    updateLineNumberAreaWidth(0);
    update();
}

void CodeEditor::setDarkThemeEnabled(bool enabled)
{
    darkTheme = enabled;
    QPalette p = palette();
    if (enabled) {
        p.setColor(QPalette::Base, QColor(30, 30, 30));
        p.setColor(QPalette::Text, QColor(220, 220, 220));
    } else {
        p.setColor(QPalette::Base, Qt::white);
        p.setColor(QPalette::Text, Qt::black);
    }
    setPalette(p);
    update();
}

int CodeEditor::lineNumberAreaWidth()
{
    if (!lineNumbers) return 0;

    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    // 计算宽度：数字宽度 + 左右边距
    int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

int CodeEditor::foldAreaWidth()
{
    if (!codeFolding) return 0;
    return 16;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    // 更新边距
    setViewportMargins(lineNumberAreaWidth() + foldAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) {
        lineNumberArea->scroll(0, dy);
    } else {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    }

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::updateFoldArea(const QRect &rect, int dy)
{
    if (dy) {
        foldArea->scroll(0, dy);
    } else {
        foldArea->update(0, rect.y(), foldArea->width(), rect.height());
    }
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect cr = contentsRect();
    int lineNumWidth = lineNumberAreaWidth();
    int foldWidth = foldAreaWidth();

    // 设置行号区域位置
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumWidth, cr.height()));

    // 设置折叠区域位置（在行号区域右边）
    foldArea->setGeometry(QRect(cr.left() + lineNumWidth, cr.top(), foldWidth, cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    updateExtraSelections();
}

void CodeEditor::updateExtraSelections()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(45, 45, 45);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    // 添加折叠区域的额外选择
    for (const FoldRegion &region : foldRegions) {
        if (region.folded) {
            QTextEdit::ExtraSelection foldSelection;
            foldSelection.format.setBackground(QColor(50, 50, 50));
            foldSelection.format.setProperty(QTextFormat::FullWidthSelection, true);

            QTextBlock block = document()->findBlockByLineNumber(region.startLine);
            QTextCursor cursor(block);
            cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor, region.endLine - region.startLine);

            foldSelection.cursor = cursor;
            extraSelections.append(foldSelection);
        }
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    if (!lineNumbers) return;

    QPainter painter(lineNumberArea);

    // 填充背景
    painter.fillRect(event->rect(), QColor(45, 45, 45));

    // 设置字体和颜色
    QFont font = this->font();
    painter.setFont(font);
    painter.setPen(QColor(150, 150, 150));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            // 绘制行号背景
            painter.fillRect(QRect(0, top, lineNumberArea->width(),
                                   fontMetrics().height()),
                             QColor(45, 45, 45));

            // 绘制行号文本
            painter.drawText(0, top, lineNumberArea->width() - 4,
                             fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::foldAreaPaintEvent(QPaintEvent *event)
{
    if (!codeFolding) return;

    QPainter painter(foldArea);
    painter.fillRect(event->rect(), QColor(35, 35, 35));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int)blockBoundingRect(block).height();

    painter.setPen(QColor(120, 120, 120));

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            // 检查这个块是否有折叠区域
            for (const FoldRegion &region : foldRegions) {
                if (region.startLine == blockNumber) {
                    QRect foldRect(0, top, foldAreaWidth(), fontMetrics().height());
                    drawFoldMarker(&painter, foldRect, region.folded);
                    break;
                }
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int)blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::drawFoldMarker(QPainter *painter, const QRect &rect, bool folded)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    if (folded) {
        painter->setBrush(QColor(80, 80, 80));
        painter->setPen(QColor(180, 180, 180));
        painter->drawRect(rect.adjusted(2, 2, -2, -2));

        painter->drawText(rect, Qt::AlignCenter, "+");
    } else {
        painter->setBrush(QColor(80, 80, 80));
        painter->setPen(QColor(180, 180, 180));
        painter->drawRect(rect.adjusted(2, 2, -2, -2));

        painter->drawText(rect, Qt::AlignCenter, "-");
    }

    painter->restore();
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    if (codeFolding && event->button() == Qt::LeftButton) {
        QRect foldRect(contentsRect().left() + lineNumberAreaWidth(), 0, foldAreaWidth(), height());
        if (foldRect.contains(event->pos())) {
            // 计算点击的行
            QTextBlock block = firstVisibleBlock();
            int blockNumber = block.blockNumber();
            int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
            int bottom = top + (int)blockBoundingRect(block).height();

            while (block.isValid() && top <= event->pos().y()) {
                if (event->pos().y() >= top && event->pos().y() <= bottom) {
                    toggleFold(blockNumber);
                    break;
                }

                block = block.next();
                top = bottom;
                bottom = top + (int)blockBoundingRect(block).height();
                ++blockNumber;
            }

            event->accept();
            return;
        }
    }

    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::paintEvent(QPaintEvent *event)
{
    QPlainTextEdit::paintEvent(event);

    if (codeFolding) {
        // 绘制折叠区域的线
        QPainter painter(viewport());
        painter.setPen(QColor(60, 60, 60));

        for (const FoldRegion &region : foldRegions) {
            if (region.folded) {
                QTextBlock startBlock = document()->findBlockByLineNumber(region.startLine);
                QTextBlock endBlock = document()->findBlockByLineNumber(region.endLine);

                if (startBlock.isValid() && endBlock.isValid()) {
                    QRect startRect = blockBoundingGeometry(startBlock).translated(contentOffset()).toRect();
                    QRect endRect = blockBoundingGeometry(endBlock).translated(contentOffset()).toRect();

                    painter.drawLine(0, startRect.bottom(), width(), startRect.bottom());
                    painter.drawLine(0, endRect.top(), width(), endRect.top());
                }
            }
        }
    }
}

void CodeEditor::toggleFold(int line)
{
    for (FoldRegion &region : foldRegions) {
        if (region.startLine == line) {
            region.folded = !region.folded;
            updateExtraSelections();
            update();
            break;
        }
    }
}

void CodeEditor::updateFoldRegions()
{
    foldRegions.clear();

    QTextBlock block = document()->firstBlock();
    QVector<int> stack;

    while (block.isValid()) {
        QString text = block.text();
        int blockNumber = block.blockNumber();

        // 检查是否有开始折叠的区域（大括号、预处理指令等）
        if (text.contains("{") ||
            (text.trimmed().startsWith("#if") && !text.trimmed().startsWith("#endif")) ||
            (text.trimmed().startsWith("#ifdef") && !text.trimmed().startsWith("#endif")) ||
            (text.trimmed().startsWith("#ifndef") && !text.trimmed().startsWith("#endif"))) {
            stack.append(blockNumber);
        }

        // 检查是否有结束折叠的区域
        if (text.contains("}") || text.trimmed().startsWith("#endif")) {
            if (!stack.isEmpty()) {
                int startLine = stack.takeLast();
                foldRegions.append(FoldRegion(startLine, blockNumber));
            }
        }

        block = block.next();
    }

    updateExtraSelections();
}

bool CodeEditor::isFolded(int line) const
{
    for (const FoldRegion &region : foldRegions) {
        if (region.startLine == line) {
            return region.folded;
        }
    }
    return false;
}

void CodeEditor::handleAutoBrackets(QKeyEvent *event)
{
    QChar key = event->text().isEmpty() ? QChar() : event->text().at(0);
    QTextCursor cursor = textCursor();

    if (key == '(') {
        cursor.insertText("()");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        event->accept();
    }
    else if (key == '[') {
        cursor.insertText("[]");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        event->accept();
    }
    else if (key == '{') {
        // 不再在这里处理左大括号，因为我们在keyPressEvent中已经处理了
        event->ignore();
    }
    else if (key == ')' || key == ']' || key == '}') {
        QTextCursor cursor = textCursor();
        if (cursor.hasSelection()) {
            // 如果有选中文本，不进行自动补全
            event->ignore();
            return;
        }

        QChar nextChar = document()->characterAt(cursor.position());
        if (nextChar == key) {
            // 如果下一个字符就是对应的右括号，直接跳过
            cursor.movePosition(QTextCursor::Right);
            setTextCursor(cursor);
            event->accept();
        } else {
            // 否则正常插入
            event->ignore();
        }
    }
    else {
        event->ignore();
    }
}

void CodeEditor::handleAutoQuotes(QKeyEvent *event)
{
    QChar key = event->text().isEmpty() ? QChar() : event->text().at(0);
    QTextCursor cursor = textCursor();

    if (key == '\"') {
        cursor.insertText("\"\"");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        event->accept();
    }
    else if (key == '\'') {
        cursor.insertText("''");
        cursor.movePosition(QTextCursor::Left);
        setTextCursor(cursor);
        event->accept();
    }
    else {
        event->ignore();
    }
}

void CodeEditor::handleAutoIndent()
{
    QTextCursor cursor = textCursor();
    QTextBlock currentBlock = cursor.block();
    QTextBlock previousBlock = currentBlock.previous();

    if (!previousBlock.isValid()) return;

    QString previousText = previousBlock.text();
    int indentLevel = 0;

    // 计算前一行开头的空格/制表符数量
    for (int i = 0; i < previousText.length(); ++i) {
        if (previousText[i].isSpace()) {
            indentLevel++;
        } else {
            break;
        }
    }

    // 获取当前行的文本
    QString currentLineText = currentBlock.text();

    // 检查当前行是否已经有右大括号
    bool hasClosingBrace = currentLineText.trimmed() == "}";

    // 如果前一行以{结尾，增加一级缩进
    QString trimmedPrevious = previousText.trimmed();
    if (trimmedPrevious.endsWith('{') && !hasClosingBrace) {
        indentLevel += indentSize;
    }

    // 插入缩进
    if (indentLevel > 0) {
        QString indent(indentLevel, ' ');
        cursor.insertText(indent);
    }

    // 如果当前行已经有右大括号，并且前一行以}结尾，减少一级缩进
    if (hasClosingBrace && trimmedPrevious.endsWith('}')) {
        cursor.movePosition(QTextCursor::StartOfLine);
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, indentSize);
        QString selectedText = cursor.selectedText();
        if (selectedText.trimmed().isEmpty() && selectedText.length() == indentSize) {
            cursor.removeSelectedText();
        }
    }
}

class Debugger : public QObject
{
    Q_OBJECT

public:
    explicit Debugger(QObject *parent = nullptr);
    ~Debugger();
    bool startDebugging(const QString &executablePath, const QString &sourcePath);
    void stopDebugging();
    void setBreakpoint(int lineNumber);
    void removeBreakpoint(int lineNumber);
    void continueExecution();
    void stepOver();
    void stepInto();
    void stepOut();
    QString getVariableValue(const QString &variableName);
    bool isRunning() const { return debuggerProcess != nullptr && debuggerProcess->state() == QProcess::Running; }

signals:
    void breakpointHit(const QString &file, int line, const QString &function);
    void debuggingFinished();
    void outputReceived(const QString &output);
    void errorReceived(const QString &error);

private slots:
    void readDebuggerOutput();
    void readDebuggerError();
    void onDebuggerFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *debuggerProcess;
    QString currentExecutable;
    QMap<int, bool> breakpoints; // line -> enabled
    void sendCommand(const QString &command);
    void parseGDBOutput(const QString &output);
};
Debugger::Debugger(QObject *parent)
    : QObject(parent), debuggerProcess(nullptr)
{
}
Debugger::~Debugger()
{
    stopDebugging();
}
bool Debugger::startDebugging(const QString &executablePath, const QString &sourcePath)
{
    if (isRunning())
    {
        stopDebugging();
    }

    debuggerProcess = new QProcess(this);
    debuggerProcess->setProcessChannelMode(QProcess::MergedChannels);
    connect(debuggerProcess, &QProcess::readyReadStandardOutput,
            this, &Debugger::readDebuggerOutput);
    connect(debuggerProcess, &QProcess::readyReadStandardError,
            this, &Debugger::readDebuggerError);
    connect(debuggerProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Debugger::onDebuggerFinished);
    currentExecutable = executablePath;
    QStringList args;
    args << "--interpreter=mi" << executablePath;
    debuggerProcess->start("gdb", args);
    if (!debuggerProcess->waitForStarted(3000)) {
        emit errorReceived(tr("无法启动调试器gdb。请确保已安装GDB。"));
        delete debuggerProcess;
        debuggerProcess = nullptr;
        return false;
    }
    sendCommand("set pagination off");
    sendCommand("set breakpoint pending on");
    for (auto it = breakpoints.begin(); it != breakpoints.end(); ++it)
    {
        if (it.value())
        {
            sendCommand(QString("break %1:%2").arg(sourcePath).arg(it.key()));
        }
    }
    emit outputReceived(tr("调试会话已启动"));
    return true;
}

void Debugger::stopDebugging()
{
    if (debuggerProcess && isRunning())
    {
        sendCommand("quit");
        debuggerProcess->waitForFinished(1000);
        debuggerProcess->kill();
        debuggerProcess->deleteLater();
        debuggerProcess = nullptr;
    }
}

void Debugger::setBreakpoint(int lineNumber)
{
    breakpoints[lineNumber] = true;
    if (isRunning())
    {
        sendCommand(QString("break %1").arg(lineNumber));
    }
}

void Debugger::removeBreakpoint(int lineNumber)
{
    breakpoints.remove(lineNumber);
    if (isRunning())
    {
        sendCommand(QString("clear %1").arg(lineNumber));
    }
}

void Debugger::continueExecution()
{
    if (isRunning())
    {
        sendCommand("continue");
    }
}

void Debugger::stepOver()
{
    if (isRunning())
    {
        sendCommand("next");
    }
}

void Debugger::stepInto()
{
    if (isRunning())
    {
        sendCommand("step");
    }
}

void Debugger::stepOut()
{
    if (isRunning())
    {
        sendCommand("finish");
    }
}

QString Debugger::getVariableValue(const QString &variableName)
{
    if (isRunning())
    {
        sendCommand(QString("print %1").arg(variableName));
    }
    return QString();
}

void Debugger::sendCommand(const QString &command)
{
    if (debuggerProcess && isRunning())
    {
        debuggerProcess->write((command + "\n").toUtf8());
        debuggerProcess->waitForBytesWritten(100);
    }
}

void Debugger::readDebuggerOutput()
{
    if (!debuggerProcess) return;
    QString output = QString::fromUtf8(debuggerProcess->readAllStandardOutput());
    parseGDBOutput(output);
    emit outputReceived(output);
}

void Debugger::readDebuggerError()
{
    if (!debuggerProcess) return;
    QString error = QString::fromUtf8(debuggerProcess->readAllStandardError());
    emit errorReceived(error);
}

void Debugger::onDebuggerFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    emit debuggingFinished();
    emit outputReceived(tr("调试会话已结束"));
}

void Debugger::parseGDBOutput(const QString &output)
{
    if (output.contains("*stopped"))
    {
        QRegularExpression regex(R"(file=\"([^\"]+)\".*line=\"(\d+)\")");
        QRegularExpressionMatch match = regex.match(output);
        if (match.hasMatch()) {
            QString file = match.captured(1);
            int line = match.captured(2).toInt();
            emit breakpointHit(file, line, "");
        }
    }
}

class CodeCompleter : public QObject
{
    Q_OBJECT

public:
    explicit CodeCompleter(QObject *parent = nullptr);
    void setEditor(CodeEditor *editor);
    QStringList getCompletions(const QString &prefix, int cursorPosition);

private:
    CodeEditor *codeEditor;
    QStringList cppKeywords;
    QStringList cppTypes;
    QStringList cppFunctions;

    void initializeKeywords();
    void extractLocalSymbols(const QString &text, int cursorPosition);
};
CodeCompleter::CodeCompleter(QObject *parent)
    : QObject(parent), codeEditor(nullptr)
{
    initializeKeywords();
}
void CodeCompleter::setEditor(CodeEditor *editor)
{
    codeEditor = editor;
}
void CodeCompleter::initializeKeywords()
{
    cppKeywords = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
        "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast",
        "continue", "co_await", "co_return", "co_yield", "decltype", "default",
        "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit",
        "export", "extern", "false", "float", "for", "friend", "goto", "if",
        "inline", "int", "long", "mutable", "namespace", "new", "noexcept", "not",
        "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected",
        "public", "register", "reinterpret_cast", "requires", "return", "short",
        "signed", "sizeof", "static", "static_assert", "static_cast", "struct",
        "switch", "template", "this", "thread_local", "throw", "true", "try",
        "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual",
        "void", "volatile", "wchar_t", "while", "xor", "xor_eq"
    };
    cppTypes = {
        "int", "float", "double", "char", "bool", "void", "short", "long",
        "signed", "unsigned", "size_t", "int8_t", "int16_t", "int32_t", "int64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t", "string", "wstring",
        "vector", "list", "map", "set", "unordered_map", "unordered_set",
        "unique_ptr", "shared_ptr", "weak_ptr", "function", "tuple", "pair"
    };
    cppFunctions = {
        "printf", "scanf", "malloc", "free", "new", "delete",
        "cout", "cin", "cerr", "endl", "vector", "string",
        "map", "set", "list", "deque", "queue", "stack"
    };
}
QStringList CodeCompleter::getCompletions(const QString &prefix, int cursorPosition)
{
    Q_UNUSED(cursorPosition)
    QStringList completions;
    for (const QString &keyword : cppKeywords)
    {
        if (keyword.startsWith(prefix, Qt::CaseInsensitive))
        {
            completions.append(keyword);
        }
    }
    for (const QString &type : cppTypes)
    {
        if (type.startsWith(prefix, Qt::CaseInsensitive))
        {
            completions.append(type);
        }
    }
    for (const QString &func : cppFunctions)
    {
        if (func.startsWith(prefix, Qt::CaseInsensitive))
        {
            completions.append(func);
        }
    }
    QStringList qtClasses = {
        "QApplication", "QMainWindow", "QWidget", "QDialog", "QLabel",
        "QPushButton", "QLineEdit", "QTextEdit", "QPlainTextEdit",
        "QVBoxLayout", "QHBoxLayout", "QGridLayout", "QFormLayout",
        "QMenu", "QMenuBar", "QToolBar", "QStatusBar", "QAction",
        "QString", "QList", "QVector", "QMap", "QHash", "QSet",
        "QFile", "QDir", "QFileInfo", "QSettings", "QProcess",
        "QTimer", "QDateTime", "QDate", "QTime", "QUrl"
    };

    for (const QString &qtClass : qtClasses)
    {
        if (qtClass.startsWith(prefix, Qt::CaseInsensitive))
        {
            completions.append(qtClass);
        }
    }
    completions.sort();
    completions.removeDuplicates();
    return completions;
}
void CodeEditor::setCompleter(CodeCompleter *comp)
{
    completer = comp;
    if (completer)
    {
        completer->setEditor(this);
    }
}
void CodeEditor::showCompletion(const QStringList &completions)
{
    if (completions.isEmpty() || !completionEnabled || !completionPopup || !completionList)
    {
        hideCompletion();
        return;
    }
    QStringListModel *model = qobject_cast<QStringListModel*>(completionList->model());
    if (!model)
    {
        model = new QStringListModel(completionPopup);
        completionList->setModel(model);
    }
    model->setStringList(completions);
    QRect rect = cursorRect();
    QPoint popupPos = mapToGlobal(QPoint(rect.left(), rect.bottom()));
    QScreen *screen = QGuiApplication::screenAt(popupPos);
    if (screen)
    {
        QRect screenGeometry = screen->availableGeometry();
        if (popupPos.x() + completionPopup->width() > screenGeometry.right())
        {
            popupPos.setX(screenGeometry.right() - completionPopup->width());
        }
        if (popupPos.y() + completionPopup->height() > screenGeometry.bottom())
        {
            popupPos.setY(rect.top() - completionPopup->height());
        }
    }
    completionPopup->move(popupPos);
    completionPopup->show();
    setFocus();
    completionList->setCurrentIndex(model->index(0, 0));
}
void CodeEditor::insertCompletion(const QString &completion)
{
    if (completion.isEmpty()) return;
    QTextCursor cursor = textCursor();
    int position = cursor.position();
    cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, completionPrefix.length());
    cursor.removeSelectedText();
    cursor.insertText(completion);
    ignoreNextKeyPress = true;
    hideCompletion();
}
void CodeEditor::hideCompletion()
{
    if (completionPopup)
    {
        completionPopup->hide();
    }
}
QString CodeEditor::getWordUnderCursor() const
{
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    return cursor.selectedText();
}
void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    if (ignoreNextKeyPress)
    {
        ignoreNextKeyPress = false;
        QPlainTextEdit::keyPressEvent(event);
        return;
    }
    if (autoBrackets)
    {
        handleAutoBrackets(event);
        if (event->isAccepted()) return;
    }
    if (autoQuotes)
    {
        handleAutoQuotes(event);
        if (event->isAccepted()) return;
    }
    QChar key = event->text().isEmpty() ? QChar() : event->text().at(0);
    if (key == '{')
    {
        QTextCursor cursor = textCursor();
        QTextBlock currentBlock = cursor.block();
        QString currentLineText = currentBlock.text();
        int indentLevel = 0;
        for (int i = 0; i < currentLineText.length(); ++i)
        {
            if (currentLineText[i].isSpace())
            {
                indentLevel++;
            }
            else
            {
                break;
            }
        }
        QString indent(indentLevel, ' ');
        QString newIndent(indentLevel + indentSize, ' ');
        int originalPosition = cursor.position();
        cursor.insertText("{\n" + newIndent + "\n" + indent + "}");
        cursor.setPosition(originalPosition);
        int newPosition = originalPosition + 1;
        newPosition += 1;
        newPosition += newIndent.length();
        cursor.setPosition(newPosition);
        setTextCursor(cursor);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Space && (event->modifiers() & Qt::ControlModifier))
    {
        if (completionEnabled && completer) {
            QString word = getWordUnderCursor();
            completionPrefix = word;
            QStringList completions = completer->getCompletions(word, textCursor().position());
            showCompletion(completions);
        }
        event->accept();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
    if (autoIndent && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
    {
        QTextCursor cursor = textCursor();
        QTextBlock currentBlock = cursor.block();
        QString currentLineText = currentBlock.text();
        if (currentLineText.trimmed() == "}")
        {
            cursor.movePosition(QTextCursor::StartOfLine);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, indentSize);
            QString selectedText = cursor.selectedText();
            if (selectedText.trimmed().isEmpty() && selectedText.length() == indentSize)
            {
                cursor.removeSelectedText();
            }
        }
        else
        {
            handleAutoIndent();
        }
    }
    if (completionEnabled && completer && !event->text().isEmpty())
    {
        QChar lastChar = event->text().at(0);
        if (lastChar.isLetterOrNumber() || lastChar == '.' || lastChar == ':' || lastChar == '>')
        {
            QString word = getWordUnderCursor();
            if (word.length() >= 1)
            {
                completionPrefix = word;
                QStringList completions = completer->getCompletions(word, textCursor().position());
                if (!completions.isEmpty())
                {
                    showCompletion(completions);
                }
                else
                {
                    hideCompletion();
                }
            }
            else
            {
                hideCompletion();
            }
        }
        else if (lastChar.isSpace() || event->key() == Qt::Key_Backspace)
        {
            hideCompletion();
        }
    }
    if (codeFolding)
    {
        updateFoldRegions();
    }
}

class IntegratedTerminal : public QDockWidget
{
    Q_OBJECT

public:
    explicit IntegratedTerminal(QWidget *parent = nullptr);
    void executeCommand(const QString &command);
    void setWorkingDirectory(const QString &dir);

public slots:
    void clear();

private slots:
    void readOutput();
    void readError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onReturnPressed();

private:
    QPlainTextEdit *terminalOutput;
    QLineEdit *terminalInput;
    QProcess *terminalProcess;
    QString currentWorkingDir;
    void setupTerminal();
    void setupChineseEnvironment();
    QByteArray encodeForWindowsTerminal(const QString &text);
    QString decodeFromWindowsTerminal(const QByteArray &data);
};
IntegratedTerminal::IntegratedTerminal(QWidget *parent)
    : QDockWidget(tr("终端"), parent), terminalProcess(nullptr)
{
    setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    setFeatures(DockWidgetMovable | DockWidgetClosable);
    setupTerminal();
}
void IntegratedTerminal::setupTerminal()
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    terminalOutput = new QPlainTextEdit(this);
    terminalOutput->setReadOnly(true);
    terminalOutput->setWordWrapMode(QTextOption::WrapAnywhere);
    terminalOutput->setStyleSheet(
        "QPlainTextEdit {"
        "    background-color: black;"
        "    color: #00FF00;"
        "    font-family: 'Consolas', 'Monospace';"
        "    font-size: 10pt;"
        "    selection-background-color: #006600;"
        "}"
        );
    terminalInput = new QLineEdit(this);
    terminalInput->setStyleSheet(
        "QLineEdit {"
        "    background-color: black;"
        "    color: #00FF00;"
        "    border: 1px solid #00FF00;"
        "    font-family: 'Consolas', 'Monospace';"
        "    font-size: 10pt;"
        "    padding: 2px;"
        "}"
        );
    layout->addWidget(terminalOutput);
    layout->addWidget(terminalInput);
    setWidget(mainWidget);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LANG", "zh_CN.UTF-8");
    env.insert("LC_ALL", "zh_CN.UTF-8");
    env.insert("PYTHONIOENCODING", "utf-8");

#ifdef Q_OS_WIN
    env.insert("CHCP", "936");
    env.insert("LANGUAGE", "zh_CN");
#endif
    terminalProcess = new QProcess(this);
    terminalProcess->setProcessEnvironment(env);
    terminalProcess->setProcessChannelMode(QProcess::MergedChannels);

#ifdef Q_OS_WIN
    terminalProcess->start("cmd.exe");
    if (terminalProcess->waitForStarted(1000))
    {
        QByteArray chcpCommand = QByteArray("chcp 936\r\n");
        terminalProcess->write(chcpCommand);
        QTimer::singleShot(500, [this]()
        {
            terminalOutput->appendPlainText(tr("终端已启动。输入命令并按Enter执行。"));
        });
    }
#else
    terminalProcess->start("/bin/bash");
    terminalOutput->appendPlainText(tr("终端已启动。输入命令并按Enter执行。"));
#endif
    connect(terminalProcess, &QProcess::readyReadStandardOutput,
            this, &IntegratedTerminal::readOutput);
    connect(terminalProcess, &QProcess::readyReadStandardError,
            this, &IntegratedTerminal::readError);
    connect(terminalProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &IntegratedTerminal::onProcessFinished);
    connect(terminalInput, &QLineEdit::returnPressed,
            this, &IntegratedTerminal::onReturnPressed);
#ifdef Q_OS_WIN
    QTimer::singleShot(1000, this, &IntegratedTerminal::setupChineseEnvironment);
#endif
}
void IntegratedTerminal::setupChineseEnvironment()
{
#ifdef Q_OS_WIN
    if (terminalProcess && terminalProcess->state() == QProcess::Running)
    {
        QByteArray chcpCommand = QByteArray("chcp 936\r\n");
        terminalProcess->write(chcpCommand);
        QByteArray fontCommand = QByteArray("powershell -Command \"Set-ItemProperty -Path 'HKCU:\\Console' -Name 'FaceName' -Value 'SimSun-ExtB'\" 2>nul\r\n");
        terminalProcess->write(fontCommand);

        terminalProcess->waitForBytesWritten(1000);
    }
#endif
}

QByteArray IntegratedTerminal::encodeForWindowsTerminal(const QString &text)
{
#ifdef Q_OS_WIN
    return text.toLocal8Bit();
#else
    return text.toUtf8();
#endif
}

QString IntegratedTerminal::decodeFromWindowsTerminal(const QByteArray &data)
{
#ifdef Q_OS_WIN
    return QString::fromLocal8Bit(data);
#else
    return QString::fromUtf8(data);
#endif
}
void IntegratedTerminal::executeCommand(const QString &command)
{
    if (terminalProcess && terminalProcess->state() == QProcess::Running)
    {
        terminalOutput->appendPlainText("$ " + command);
        QByteArray commandData = encodeForWindowsTerminal(command + "\r\n");
        terminalProcess->write(commandData);
        terminalProcess->waitForBytesWritten(1000);
    }
}
void IntegratedTerminal::setWorkingDirectory(const QString &dir)
{
    currentWorkingDir = dir;
    if (!dir.isEmpty())
    {
        QString command = "cd /d \"" + dir + "\"";
        executeCommand(command);
    }
}
void IntegratedTerminal::clear()
{
    terminalOutput->clear();
}
void IntegratedTerminal::readOutput()
{
    if (terminalProcess)
    {
        QByteArray outputData = terminalProcess->readAllStandardOutput();
        QString output = decodeFromWindowsTerminal(outputData);
        output.remove(QRegularExpression("\\x1B\\[[0-9;]*[A-Za-z]"));
        output.remove(QRegularExpression("[\\x00-\\x09\\x0B-\\x1F\\x7F]"));
        if (!output.trimmed().isEmpty())
        {
            terminalOutput->appendPlainText(output);
            terminalOutput->verticalScrollBar()->setValue(
                terminalOutput->verticalScrollBar()->maximum());
        }
    }
}
void IntegratedTerminal::readError()
{
    if (terminalProcess)
    {
        QByteArray errorData = terminalProcess->readAllStandardError();
        QString error = decodeFromWindowsTerminal(errorData);
        error.remove(QRegularExpression("\\x1B\\[[0-9;]*[A-Za-z]"));
        error.remove(QRegularExpression("[\\x00-\\x09\\x0B-\\x1F\\x7F]"));
        if (!error.trimmed().isEmpty())
        {
            terminalOutput->appendPlainText(tr("错误: ") + error);
        }
    }
}
void IntegratedTerminal::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    terminalOutput->appendPlainText(tr("终端进程已结束"));
}
void IntegratedTerminal::onReturnPressed()
{
    QString command = terminalInput->text();
    if (!command.isEmpty())
    {
        executeCommand(command);
        terminalInput->clear();
    }
}

class SnippetManager : public QDockWidget
{
    Q_OBJECT

public:
    explicit SnippetManager(QWidget *parent = nullptr);
    void addSnippet(const QString &name, const QString &code, const QString &category = "General");
    void removeSnippet(const QString &name);
    QString getSnippet(const QString &name) const;
    QStringList getCategories() const;
    QStringList getSnippetsInCategory(const QString &category) const;

public slots:
    void insertSnippet(const QString &name);

signals:
    void snippetInserted(const QString &code);

private slots:
    void onSnippetDoubleClicked(QListWidgetItem *item);
    void onAddSnippetClicked();
    void onEditSnippetClicked();
    void onDeleteSnippetClicked();
    void onCategoryChanged(const QString &category);

private:
    QMap<QString, QMap<QString, QString>> snippets; // category -> (name -> code)
    QListWidget *snippetList;
    QComboBox *categoryCombo;
    QLineEdit *searchEdit;
    void setupUI();
    void loadSnippets();
    void saveSnippets();
    void refreshSnippetList();
};
SnippetManager::SnippetManager(QWidget *parent)
    : QDockWidget(tr("代码片段"), parent)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(DockWidgetMovable | DockWidgetClosable);
    setupUI();
    loadSnippets();
}
void SnippetManager::setupUI()
{
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText(tr("搜索代码片段..."));
    layout->addWidget(searchEdit);
    QHBoxLayout *categoryLayout = new QHBoxLayout;
    categoryLayout->addWidget(new QLabel(tr("类别:")));
    categoryCombo = new QComboBox(this);
    categoryLayout->addWidget(categoryCombo);
    layout->addLayout(categoryLayout);
    snippetList = new QListWidget(this);
    layout->addWidget(snippetList);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton(tr("添加"), this);
    QPushButton *editButton = new QPushButton(tr("编辑"), this);
    QPushButton *deleteButton = new QPushButton(tr("删除"), this);
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    layout->addLayout(buttonLayout);
    setWidget(mainWidget);
    connect(snippetList, &QListWidget::itemDoubleClicked,
            this, &SnippetManager::onSnippetDoubleClicked);
    connect(addButton, &QPushButton::clicked,
            this, &SnippetManager::onAddSnippetClicked);
    connect(editButton, &QPushButton::clicked,
            this, &SnippetManager::onEditSnippetClicked);
    connect(deleteButton, &QPushButton::clicked,
            this, &SnippetManager::onDeleteSnippetClicked);
    connect(categoryCombo, &QComboBox::currentTextChanged,
            this, &SnippetManager::onCategoryChanged);
    connect(searchEdit, &QLineEdit::textChanged,
            this, [this](const QString &text) { Q_UNUSED(text) refreshSnippetList(); });
    categoryCombo->addItems({"General", "C++", "Qt", "Algorithms", "Data Structures"});
}
void SnippetManager::addSnippet(const QString &name, const QString &code, const QString &category)
{
    QString cat = category.isEmpty() ? "General" : category;
    snippets[cat][name] = code;
    saveSnippets();
    refreshSnippetList();
}
void SnippetManager::removeSnippet(const QString &name)
{
    for (auto it = snippets.begin(); it != snippets.end(); ++it)
    {
        if (it.value().contains(name))
        {
            it.value().remove(name);
            break;
        }
    }
    saveSnippets();
    refreshSnippetList();
}
QString SnippetManager::getSnippet(const QString &name) const
{
    for (auto it = snippets.constBegin(); it != snippets.constEnd(); ++it)
    {
        if (it.value().contains(name))
        {
            return it.value().value(name);
        }
    }
    return QString();
}
QStringList SnippetManager::getCategories() const
{
    return snippets.keys();
}
QStringList SnippetManager::getSnippetsInCategory(const QString &category) const
{
    if (snippets.contains(category))
    {
        return snippets[category].keys();
    }
    return QStringList();
}
void SnippetManager::insertSnippet(const QString &name)
{
    QString code = getSnippet(name);
    if (!code.isEmpty())
    {
        emit snippetInserted(code);
    }
}
void SnippetManager::onSnippetDoubleClicked(QListWidgetItem *item)
{
    if (item)
    {
        insertSnippet(item->text());
    }
}
void SnippetManager::onAddSnippetClicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("添加代码片段"),
                                         tr("片段名称:"), QLineEdit::Normal, "", &ok);
    if (ok && !name.isEmpty())
    {
        QString code = QInputDialog::getMultiLineText(this, tr("添加代码片段"),
                                                      tr("代码内容:"), "", &ok);
        if (ok && !code.isEmpty())
        {
            QString category = categoryCombo->currentText();
            addSnippet(name, code, category);
        }
    }
}
void SnippetManager::onEditSnippetClicked()
{
    QListWidgetItem *item = snippetList->currentItem();
    if (!item) return;
    QString oldName = item->text();
    QString oldCode = getSnippet(oldName);
    bool ok;
    QString newName = QInputDialog::getText(this, tr("编辑代码片段"),
                                            tr("片段名称:"), QLineEdit::Normal, oldName, &ok);
    if (ok && !newName.isEmpty())
    {
        QString newCode = QInputDialog::getMultiLineText(this, tr("编辑代码片段"),
                                                         tr("代码内容:"), oldCode, &ok);
        if (ok)
        {
            removeSnippet(oldName);
            QString category = categoryCombo->currentText();
            addSnippet(newName, newCode, category);
        }
    }
}
void SnippetManager::onDeleteSnippetClicked()
{
    QListWidgetItem *item = snippetList->currentItem();
    if (!item) return;
    QString name = item->text();
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("确认删除"), tr("确定要删除代码片段\"%1\"吗?").arg(name));
    if (reply == QMessageBox::Yes)
    {
        removeSnippet(name);
    }
}
void SnippetManager::onCategoryChanged(const QString &category)
{
    Q_UNUSED(category)
    refreshSnippetList();
}
void SnippetManager::loadSnippets()
{
    QSettings settings("CompilerIDE", "Compiler IDE " + IDE_VERSION);
    settings.beginGroup("Snippets");
    QStringList categories = settings.childGroups();
    for (const QString &category : categories)
    {
        settings.beginGroup(category);
        QStringList keys = settings.childKeys();
        for (const QString &key : keys)
        {
            snippets[category][key] = settings.value(key).toString();
        }
        settings.endGroup();
    }
    settings.endGroup();
    if (snippets.isEmpty())
    {
        addSnippet("Hello World",
                   "#include <iostream>\n\n"
                   "using namespace std;\n\n"
                   "int main() {\n"
                   "    cout << \"Hello, World!\" << endl;\n"
                   "    return 0;\n"
                   "}", "C++");

        addSnippet("For Loop",
                   "for (int i = 0; i < count; i++) {\n"
                   "    // 你的代码在这里\n"
                   "}", "C++");

        addSnippet("Class Template",
                   "class MyClass {\n"
                   "public:\n"
                   "    MyClass();\n"
                   "    ~MyClass();\n"
                   "    \n"
                   "private:\n"
                   "    // 私有成员\n"
                   "};", "C++");
    }
    refreshSnippetList();
}
void SnippetManager::saveSnippets()
{
    QSettings settings("CompilerIDE", "Compiler IDE " + IDE_VERSION);
    settings.beginGroup("Snippets");
    settings.remove("");
    for (auto catIt = snippets.begin(); catIt != snippets.end(); ++catIt)
    {
        settings.beginGroup(catIt.key());
        for (auto snippetIt = catIt.value().begin(); snippetIt != catIt.value().end(); ++snippetIt)
        {
            settings.setValue(snippetIt.key(), snippetIt.value());
        }
        settings.endGroup();
    }
    settings.endGroup();
}
void SnippetManager::refreshSnippetList()
{
    snippetList->clear();
    QString category = categoryCombo->currentText();
    QString searchText = searchEdit->text().toLower();
    if (snippets.contains(category))
    {
        const QMap<QString, QString> &categorySnippets = snippets[category];
        for (auto it = categorySnippets.begin(); it != categorySnippets.end(); ++it)
        {
            if (searchText.isEmpty() || it.key().toLower().contains(searchText))
            {
                snippetList->addItem(it.key());
            }
        }
    }
}

class ProjectManager : public QDockWidget
{
    Q_OBJECT

public:
    explicit ProjectManager(QWidget *parent = nullptr);
    void setRootPath(const QString &path);
    QString getCurrentFile() const;
    void refresh();

signals:
    void fileSelected(const QString &filePath);
    void fileOpened(const QString &filePath);

private slots:
    void onFileClicked(const QModelIndex &index);
    void onFileDoubleClicked(const QModelIndex &index);
    void showContextMenu(const QPoint &point);
    void newFile();
    void newFolder();
    void rename();
    void deleteFile();
    void openInExplorer();
    void copyPath();
    void copyRelativePath();

private:
    QFileSystemModel *model;
    QTreeView *treeView;
    QString currentPath;
    void createActions();
    QAction *newFileAction;
    QAction *newFolderAction;
    QAction *renameAction;
    QAction *deleteAction;
    QAction *refreshAction;
    QAction *openInExplorerAction;
    QAction *copyPathAction;
    QAction *copyRelativePathAction;
};
ProjectManager::ProjectManager(QWidget *parent)
    : QDockWidget(tr("项目管理器"), parent)
{
    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    setFeatures(DockWidgetMovable | DockWidgetClosable);
    QToolBar *toolbar = new QToolBar(this);
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    createActions();
    toolbar->addAction(newFileAction);
    toolbar->addAction(newFolderAction);
    toolbar->addAction(deleteAction);
    toolbar->addAction(refreshAction);
    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::AllDirs);
    model->setNameFilters(QStringList() << "*.cpp" << "*.h" << "*.hpp" << "*.cc" << "*.cxx" << "*.c++" << "*.hh" << "*.hxx" << "*.h++");
    model->setNameFilterDisables(false);
    treeView = new QTreeView(this);
    treeView->setModel(model);
    treeView->setRootIsDecorated(true);
    treeView->setAlternatingRowColors(true);
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    treeView->setDragDropMode(QAbstractItemView::InternalMove);
    treeView->setDefaultDropAction(Qt::MoveAction);
    treeView->setDragEnabled(true);
    treeView->setAcceptDrops(true);
    treeView->setDropIndicatorShown(true);
    QWidget *titleWidget = new QWidget(this);
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    titleLayout->setContentsMargins(5, 2, 5, 2);
    titleLayout->addWidget(new QLabel(tr("项目管理器")));
    titleLayout->addStretch();
    titleLayout->addWidget(toolbar);
    setTitleBarWidget(titleWidget);
    setWidget(treeView);
    connect(treeView, &QTreeView::clicked, this, &ProjectManager::onFileClicked);
    connect(treeView, &QTreeView::doubleClicked, this, &ProjectManager::onFileDoubleClicked);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &ProjectManager::showContextMenu);
    QShortcut *deleteShortcut = new QShortcut(QKeySequence::Delete, treeView);
    connect(deleteShortcut, &QShortcut::activated, this, &ProjectManager::deleteFile);
    QShortcut *renameShortcut = new QShortcut(QKeySequence(Qt::Key_F2), treeView);
    connect(renameShortcut, &QShortcut::activated, this, &ProjectManager::rename);
    QShortcut *refreshShortcut = new QShortcut(QKeySequence::Refresh, treeView);
    connect(refreshShortcut, &QShortcut::activated, this, &ProjectManager::refresh);
}
void ProjectManager::createActions()
{
    newFileAction = new QAction(QIcon(":/icons/newfile.png"), tr("新建文件"), this);
    newFileAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    newFileAction->setStatusTip(tr("创建新文件"));
    connect(newFileAction, &QAction::triggered, this, &ProjectManager::newFile);

    newFolderAction = new QAction(QIcon(":/icons/newfolder.png"), tr("新建文件夹"), this);
    newFolderAction->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    newFolderAction->setStatusTip(tr("创建新文件夹"));
    connect(newFolderAction, &QAction::triggered, this, &ProjectManager::newFolder);

    renameAction = new QAction(QIcon(":/icons/rename.png"), tr("重命名"), this);
    renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    renameAction->setStatusTip(tr("重命名选中的文件或文件夹"));
    connect(renameAction, &QAction::triggered, this, &ProjectManager::rename);

    deleteAction = new QAction(QIcon(":/icons/delete.png"), tr("删除"), this);
    deleteAction->setShortcut(QKeySequence::Delete);
    deleteAction->setStatusTip(tr("删除选中的文件或文件夹"));
    connect(deleteAction, &QAction::triggered, this, &ProjectManager::deleteFile);

    refreshAction = new QAction(QIcon(":/icons/refresh.png"), tr("刷新"), this);
    refreshAction->setShortcut(QKeySequence::Refresh);
    refreshAction->setStatusTip(tr("刷新项目视图"));
    connect(refreshAction, &QAction::triggered, this, &ProjectManager::refresh);

    openInExplorerAction = new QAction(QIcon(":/icons/open.png"), tr("在资源管理器中打开"), this);
    openInExplorerAction->setStatusTip(tr("在系统文件管理器中打开"));
    connect(openInExplorerAction, &QAction::triggered, this, &ProjectManager::openInExplorer);

    copyPathAction = new QAction(tr("复制完整路径"), this);
    copyPathAction->setStatusTip(tr("复制文件或文件夹的完整路径"));
    connect(copyPathAction, &QAction::triggered, this, &ProjectManager::copyPath);

    copyRelativePathAction = new QAction(tr("复制相对路径"), this);
    copyRelativePathAction->setStatusTip(tr("复制文件或文件夹相对于项目根目录的路径"));
    connect(copyRelativePathAction, &QAction::triggered, this, &ProjectManager::copyRelativePath);
}
void ProjectManager::setRootPath(const QString &path)
{
    // 如果路径为空，清空模型并返回
    if (path.isEmpty()) {
        model->setRootPath("");
        treeView->setRootIndex(QModelIndex());
        setWindowTitle(tr("项目管理器"));
        currentPath = "";
        return;
    }
    currentPath = path;
    QModelIndex index = model->setRootPath(path);
    treeView->setRootIndex(index);
    treeView->setColumnWidth(0, 250);
    treeView->setColumnHidden(1, true);
    treeView->setColumnHidden(2, true);
    treeView->setColumnHidden(3, false);
    treeView->setColumnWidth(3, 120);
    for (int i = 0; i < model->rowCount(index); ++i)
    {
        QModelIndex child = model->index(i, 0, index);
        if (model->isDir(child))
        {
            treeView->expand(child);
        }
    }
    setWindowTitle(tr("项目管理器 - %1").arg(QDir(path).dirName()));
}
void ProjectManager::refresh()
{
    treeView->setRootIndex(model->setRootPath(currentPath));
}
QString ProjectManager::getCurrentFile() const
{
    QModelIndex index = treeView->currentIndex();
    if (index.isValid())
    {
        return model->filePath(index);
    }
    return QString();
}
void ProjectManager::onFileClicked(const QModelIndex &index)
{
    if (!model->isDir(index))
    {
        QString filePath = model->filePath(index);
        emit fileSelected(filePath);
    }
}
void ProjectManager::onFileDoubleClicked(const QModelIndex &index)
{
    if (!model->isDir(index))
    {
        QString filePath = model->filePath(index);
        emit fileOpened(filePath);
    }
}
void ProjectManager::showContextMenu(const QPoint &point)
{
    QModelIndex index = treeView->indexAt(point);
    QMenu menu;
    if (index.isValid())
    {
        if (!model->isDir(index))
        {
            menu.addAction(openInExplorerAction);
            menu.addSeparator();
        }
        menu.addAction(renameAction);
        menu.addAction(deleteAction);
        menu.addSeparator();
        menu.addAction(copyPathAction);
        menu.addAction(copyRelativePathAction);
    }
    else
    {
        menu.addAction(newFileAction);
        menu.addAction(newFolderAction);
        menu.addSeparator();
        menu.addAction(refreshAction);
    }
    menu.exec(treeView->mapToGlobal(point));
}
void ProjectManager::newFile()
{
    QModelIndex index = treeView->currentIndex();
    QString path;
    if (index.isValid() && model->isDir(index))
    {
        path = model->filePath(index);
    }
    else if (index.isValid())
    {
        path = model->filePath(index.parent());
    }
    else
    {
        path = currentPath;
    }
    bool ok;
    QString fileName = QInputDialog::getText(this, tr("新建文件"), tr("文件名:"),
                                             QLineEdit::Normal, "newfile.cpp", &ok);
    if (ok && !fileName.isEmpty())
    {
        if (!fileName.contains('.'))
        {
            fileName += ".cpp";
        }
        QFile file(QDir(path).filePath(fileName));
        if (file.exists())
        {
            QMessageBox::warning(this, tr("错误"), tr("文件已存在"));
            return;
        }
        if (file.open(QIODevice::WriteOnly))
        {
            if (fileName.endsWith(".cpp") || fileName.endsWith(".h") ||
                fileName.endsWith(".hpp") || fileName.endsWith(".cxx"))
            {
                QTextStream stream(&file);
                stream << "// " << fileName << "\n";
                stream << "// Created: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";
                if (fileName.endsWith(".h") || fileName.endsWith(".hpp"))
                {
                    QString guardName = fileName.toUpper().replace(".", "_").replace("-", "_");
                    stream << "#ifndef " << guardName << "\n";
                    stream << "#define " << guardName << "\n\n";
                    stream << "// 你的代码在这里\n\n";
                    stream << "#endif // " << guardName << "\n";
                }
                else
                {
                    stream << "#include <iostream>\n\n";
                    stream << "using namespace std;\n\n";
                    stream << "int main() {\n";
                    stream << "    cout << \"Hello, World!\" << endl;\n";
                    stream << "    return 0;\n";
                    stream << "}\n";
                }
            }
            file.close();
            refresh();
            QModelIndex newIndex = model->index(QDir(path).filePath(fileName));
            if (newIndex.isValid())
            {
                treeView->setCurrentIndex(newIndex);
                treeView->scrollTo(newIndex);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法创建文件"));
        }
    }
}
void ProjectManager::newFolder()
{
    QModelIndex index = treeView->currentIndex();
    QString path;
    if (index.isValid() && model->isDir(index))
    {
        path = model->filePath(index);
    }
    else if (index.isValid())
    {
        path = model->filePath(index.parent());
    }
    else
    {
        path = currentPath;
    }
    bool ok;
    QString folderName = QInputDialog::getText(this, tr("新建文件夹"), tr("文件夹名:"),
                                               QLineEdit::Normal, "NewFolder", &ok);
    if (ok && !folderName.isEmpty())
    {
        QDir dir(path);
        if (dir.mkdir(folderName))
        {
            refresh();
            QModelIndex newIndex = model->index(dir.filePath(folderName));
            if (newIndex.isValid())
            {
                treeView->setCurrentIndex(newIndex);
                treeView->scrollTo(newIndex);
                treeView->expand(newIndex);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法创建文件夹"));
        }
    }
}
void ProjectManager::rename()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) return;
    QString oldPath = model->filePath(index);
    QString name = model->fileName(index);
    bool ok;
    QString newName = QInputDialog::getText(this, tr("重命名"), tr("新名称:"),
                                            QLineEdit::Normal, name, &ok);
    if (ok && !newName.isEmpty())
    {
        QDir dir = QFileInfo(oldPath).absoluteDir();
        QString newPath = dir.absoluteFilePath(newName);
        if (QFile::exists(newPath))
        {
            QMessageBox::warning(this, tr("错误"), tr("名称已存在"));
            return;
        }
        if (QFile::rename(oldPath, newPath))
        {
            refresh();
            QModelIndex newIndex = model->index(newPath);
            if (newIndex.isValid())
            {
                treeView->setCurrentIndex(newIndex);
                treeView->scrollTo(newIndex);
            }
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法重命名"));
        }
    }
}
void ProjectManager::deleteFile()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = model->filePath(index);
    QString name = model->fileName(index);
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("确认删除"), tr("确定要删除\"%1\"吗?").arg(name),
                                  QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        bool success = false;
        if (model->isDir(index))
        {
            QDir dir(path);
            success = dir.removeRecursively();
        }
        else
        {
            success = QFile::remove(path);
        }
        if (success)
        {
            refresh();
        }
        else
        {
            QMessageBox::warning(this, tr("错误"), tr("无法删除"));
        }
    }
}
void ProjectManager::openInExplorer()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = model->filePath(index);
#ifdef Q_OS_WIN
    QStringList args;
    args << "/select," << QDir::toNativeSeparators(path);
    QProcess::startDetached("explorer", args);
#elif defined(Q_OS_MAC)
    QProcess::execute("open", QStringList() << "-R" << path);
#else
    QFileInfo fileInfo(path);
    QProcess::execute("xdg-open", QStringList() << fileInfo.absolutePath());
#endif
}
void ProjectManager::copyPath()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = model->filePath(index);
    QApplication::clipboard()->setText(path);
}
void ProjectManager::copyRelativePath()
{
    QModelIndex index = treeView->currentIndex();
    if (!index.isValid()) return;
    QString path = model->filePath(index);
    QDir projectDir(currentPath);
    QString relativePath = projectDir.relativeFilePath(path);
    QApplication::clipboard()->setText(relativePath);
}

class CompilerIDE : public QMainWindow
{
    Q_OBJECT

public:
    CompilerIDE();
    ~CompilerIDE();

public:
    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QStringList recentFiles;
    void addToRecentFiles(const QString &filePath);
    void loadRecentFiles();
    void saveRecentFiles();

private:
    void setTheme(bool dark);

private slots:
    void newFile();
    void open();
    void openProject();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
    void compile();
    void run();
    void compileAndRun();
    void clearOutput();
    void compilerFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void readCompilerOutput();
    void readCompilerError();
    void showErrorInCode(const QModelIndex &index);
    void updateStatusBar();
    void changeCppStandard(int index);
    void showSettings();
    void applySettings(const QString &compilerPath, bool autoBrackets, bool autoQuotes, bool autoIndent, int indentSize, bool codeFolding, bool lineNumbers, bool darkTheme, bool enableAI, const QString &aiEndpoint, const QString &aiKey);
    void toggleComment();
    void showDocumentation();
    void showWelcomeMessage();
    void runProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void readRunOutput();
    void onFileSelected(const QString &filePath);
    void closeTab(int index);
    void currentTabChanged(int index);
    void find();
    void replace();
    void findNext();
    void findPrevious();
    void gotoLine();
    void showProjectManager();
    void showFileBrowser();
    void showSymbolBrowser();
    void showOutline();
    void zoomIn();
    void zoomOut();
    void resetZoom();
    void showAICompletion();

private:
    QString detectCompilerType(const QString &compilerPath);
    void parseCompilerOutput(const QString &output);
    HANDLE hProcess;
    void createFindDialog();
    bool performFind(bool forward, bool showMessage = true);
    void updateFindDialog();

private:
    AccountManager *accountManager;
    QAction *loginAct;
    QAction *logoutAct;
    QAction *registerAct;
    QAction *userInfoAct;
    QLabel *userStatusLabel;

private slots:
    void showLoginDialog();
    void logout();
    void showUserInfo();
    void updateUserStatus();
    void onLoginStatusChanged(bool loggedIn);
    void onUserLoggedIn(const QString &username);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void runExecutable(const QString &programPath);
    CodeEditor* currentEditor() const;
    CodeEditor* editorAt(int index) const;
    int findEditor(const QString &fileName) const;
    QTabWidget *tabWidget;
    QPlainTextEdit *outputEdit;
    QListView *errorListView;
    QStringListModel *errorListModel;
    QDockWidget *outputDock;
    QDockWidget *errorDock;
    ProjectManager *projectManager;
    QDockWidget *projectDock;
    QDockWidget *fileBrowserDock;
    QDockWidget *symbolBrowserDock;
    QDockWidget *outlineDock;
    QProcess *compileProcess;
    QProcess *runProcess;
    QString curFile;
    QString compilerPath;
    QString compilerType;
    QString cppStandard;
    bool autoBrackets;
    bool autoQuotes;
    bool autoIndent;
    int indentSize;
    bool codeFolding;
    bool lineNumbers;
    bool darkTheme;
    bool enableAI;
    QString aiEndpoint;
    QString aiKey;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *buildMenu;
    QMenu *viewMenu;
    QMenu *toolsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *buildToolBar;
    QToolBar *editToolBar;
    QToolBar *viewToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *openProjectAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAct;
    QAction *closeAllAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *toggleCommentAct;
    QAction *findAct;
    QAction *replaceAct;
    QAction *findNextAct;
    QAction *findPreviousAct;
    QAction *gotoLineAct;
    QAction *compileAct;
    QAction *runAct;
    QAction *compileRunAct;
    QAction *clearOutputAct;
    QAction *settingsAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *documentationAct;
    QAction *welcomeAct;
    QAction *projectManagerAct;
    QAction *fileBrowserAct;
    QAction *symbolBrowserAct;
    QAction *outlineAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *resetZoomAct;
    QAction *aiCompletionAct;
    QComboBox *cppStandardCombo;
    QDialog *findDialog;
    QLineEdit *findEdit;
    QCheckBox *caseSensitiveCheck;
    QCheckBox *wholeWordCheck;
    QCheckBox *regexCheck;
    QPushButton *findNextButton;
    QPushButton *findPrevButton;
    QPushButton *replaceButton;
    QPushButton *replaceAllButton;
    QLineEdit *replaceEdit;

private:
    Debugger *debugger;
    CodeCompleter *codeCompleter;
    IntegratedTerminal *terminal;
    SnippetManager *snippetManager;
    QDockWidget *terminalDock;
    QDockWidget *snippetDock;
    QAction *debugStartAct;
    QAction *debugStopAct;
    QAction *debugContinueAct;
    QAction *debugStepOverAct;
    QAction *debugStepIntoAct;
    QAction *debugStepOutAct;
    QAction *debugToggleBreakpointAct;
    QAction *terminalShowAct;
    QAction *snippetShowAct;

signals:
    void compilationFinished(bool success);
};
CompilerIDE::CompilerIDE()
    : compileProcess(nullptr), runProcess(nullptr),
    cppStandard("c++17"), autoBrackets(true), autoQuotes(true),
    autoIndent(true), indentSize(4), codeFolding(true), lineNumbers(true),
    darkTheme(true), enableAI(false), debugger(nullptr), codeCompleter(nullptr),
    terminal(nullptr), snippetManager(nullptr)
{
    QApplication::setStyle(new ModernStyle);
    loadRecentFiles();
    setWindowTitle("Compiler IDE 2.8.6");
    setUnifiedTitleAndToolBarOnMac(true);
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    setCentralWidget(tabWidget);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &CompilerIDE::closeTab);
    connect(tabWidget, &QTabWidget::currentChanged, this, &CompilerIDE::currentTabChanged);
    createDockWindows();
    debugger = new Debugger(this);
    codeCompleter = new CodeCompleter(this);
    terminal = new IntegratedTerminal(this);
    snippetManager = new SnippetManager(this);
    accountManager = new AccountManager(this);
    connect(accountManager, &AccountManager::loginStatusChanged,
            this, &CompilerIDE::onLoginStatusChanged);
    connect(accountManager, &AccountManager::userLoggedIn,
            this, &CompilerIDE::onUserLoggedIn);
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createFindDialog();
    readSettings();
    setTheme(darkTheme);
    compilerType = detectCompilerType(compilerPath);
    terminalDock = new QDockWidget(tr("终端"), this);
    terminalDock->setWidget(terminal);
    terminalDock->setVisible(false);
    addDockWidget(Qt::BottomDockWidgetArea, terminalDock);
    snippetDock = new QDockWidget(tr("代码片段"), this);
    snippetDock->setWidget(snippetManager);
    snippetDock->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, snippetDock);
    connect(debugger, &Debugger::breakpointHit, this, [this](const QString &file, int line, const QString &function)
    {
        outputEdit->appendPlainText(tr("断点命中: %1 第 %2 行").arg(file).arg(line));
    });
    connect(debugger, &Debugger::outputReceived, this, [this](const QString &output)
    {
        outputEdit->appendPlainText(tr("[调试] ") + output);
    });
    connect(snippetManager, &SnippetManager::snippetInserted, this, [this](const QString &code)
    {
        CodeEditor *editor = currentEditor();
        if (editor)
        {
            editor->insertPlainText(code);
        }
    });
#ifdef Q_OS_WIN
    if (compilerPath.isEmpty())
    {
        compilerPath = "g++.exe";
    }
#else
    if (compilerPath.isEmpty())
    {
        compilerPath = "g++";
    }
#endif
    updateStatusBar();
    QString initialDir = QDir::currentPath();
    terminal->setWorkingDirectory(initialDir);
}
CompilerIDE::~CompilerIDE()
{
    if (compileProcess)
    {
        compileProcess->kill();
        compileProcess->waitForFinished();
        delete compileProcess;
    }
    if (runProcess)
    {
        runProcess->kill();
        runProcess->waitForFinished();
        delete runProcess;
    }
    delete debugger;
    delete codeCompleter;
    delete terminal;
    delete snippetManager;
    delete accountManager;

}
void CompilerIDE::setTheme(bool dark)
{
    if (dark)
    {
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        QApplication::setPalette(darkPalette);
        QPalette outputPalette = outputEdit->palette();
        outputPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        outputPalette.setColor(QPalette::Text, QColor(220, 220, 220));
        outputEdit->setPalette(outputPalette);
        QPalette errorPalette = errorListView->palette();
        errorPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        errorPalette.setColor(QPalette::Text, QColor(220, 220, 220));
        errorListView->setPalette(errorPalette);
    }
    else
    {
        QApplication::setPalette(style()->standardPalette());
        QPalette outputPalette = outputEdit->palette();
        outputPalette.setColor(QPalette::Base, Qt::white);
        outputPalette.setColor(QPalette::Text, Qt::black);
        outputEdit->setPalette(outputPalette);
        QPalette errorPalette = errorListView->palette();
        errorPalette.setColor(QPalette::Base, Qt::white);
        errorPalette.setColor(QPalette::Text, Qt::black);
        errorListView->setPalette(errorPalette);
    }
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        CodeEditor *editor = editorAt(i);
        if (editor)
        {
            editor->setDarkThemeEnabled(dark);
        }
    }
    update();
}
void CompilerIDE::addToRecentFiles(const QString &filePath)
{
    if (filePath.isEmpty() || !QFile::exists(filePath))
    {
        return;
    }
    recentFiles.removeAll(filePath);
    recentFiles.prepend(filePath);
    while (recentFiles.size() > 10)
    {
        recentFiles.removeLast();
    }

    saveRecentFiles();
}
void CompilerIDE::loadRecentFiles()
{
    QSettings settings("CompilerIDE", "Compiler IDE 2.8.6");
    recentFiles = settings.value("recentFiles").toStringList();
}
void CompilerIDE::saveRecentFiles()
{
    QSettings settings("CompilerIDE", "Compiler IDE 2.8.6");
    settings.setValue("recentFiles", recentFiles);
}
QString CompilerIDE::detectCompilerType(const QString &compilerPath)
{
    if (compilerPath.isEmpty())
    {
        QStringList possiblePaths;

#ifdef Q_OS_WIN
        possiblePaths << "C:/msys64/mingw64/bin/g++.exe"
                      << "C:/msys64/ucrt64/bin/g++.exe"
                      << "C:/msys64/clang64/bin/g++.exe"
                      << "C:/MinGW/bin/g++.exe"
                      << "g++.exe";
#else
        possiblePaths << "/usr/bin/g++"
                      << "/usr/local/bin/g++"
                      << "/opt/homebrew/bin/g++"
                      << "g++";
#endif
        for (const QString &path : possiblePaths)
        {
            if (QFile::exists(path))
            {
                this->compilerPath = path;
                break;
            }
        }
        if (this->compilerPath.isEmpty())
        {
#ifdef Q_OS_WIN
            return "gcc";
#else
            return "gcc";
#endif
        }
    }
    QFileInfo fileInfo(compilerPath);
    QString fileName = fileInfo.fileName().toLower();
    if (fileName.contains("g++") || fileName.contains("gcc") || fileName.contains("mingw"))
    {
        return "gcc";
    }
    else if (fileName.contains("cl") || fileName.contains("msvc"))
    {
        return "msvc";
    }
    else if (fileName.contains("clang"))
    {
        return "clang";
    }
    QProcess process;
    process.start(compilerPath, QStringList() << "--version");
    if (process.waitForFinished(3000)) {
        QString output = QString::fromLocal8Bit(process.readAllStandardOutput());
        if (output.contains("gcc", Qt::CaseInsensitive) ||
            output.contains("mingw", Qt::CaseInsensitive))
        {
            return "gcc";
        }
        else if (output.contains("clang", Qt::CaseInsensitive))
        {
            return "clang";
        }
        else if (output.contains("microsoft", Qt::CaseInsensitive))
        {
            return "msvc";
        }
    }

#ifdef Q_OS_WIN
    return "gcc";
#else
    return "gcc";
#endif
}
void CompilerIDE::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        CodeEditor *editor = editorAt(i);
        if (editor && editor->document()->isModified())
        {
            tabWidget->setCurrentIndex(i);
            if (!maybeSave())
            {
                event->ignore();
                return;
            }
        }
    }
    writeSettings();
    event->accept();
}
void CompilerIDE::createActions()
{
    newAct = new QAction(tr("新建(&N)"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("创建新文件"));
    connect(newAct, &QAction::triggered, this, &CompilerIDE::newFile);

    openAct = new QAction(tr("打开(&O)..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("打开现有文件"));
    connect(openAct, &QAction::triggered, this, &CompilerIDE::open);

    openProjectAct = new QAction(tr("打开项目(&P)..."), this);
    openProjectAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_O));
    openProjectAct->setStatusTip(tr("打开项目文件夹"));
    connect(openProjectAct, &QAction::triggered, this, &CompilerIDE::openProject);

    saveAct = new QAction(tr("保存(&S)"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("保存文档到磁盘"));
    connect(saveAct, &QAction::triggered, this, &CompilerIDE::save);

    saveAsAct = new QAction(tr("另存为(&A)..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("以新名称保存文档"));
    connect(saveAsAct, &QAction::triggered, this, &CompilerIDE::saveAs);

    closeAct = new QAction(tr("关闭(&C)"), this);
    closeAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    closeAct->setStatusTip(tr("关闭当前文件"));
    connect(closeAct, &QAction::triggered, [this]() { closeTab(tabWidget->currentIndex()); });

    closeAllAct = new QAction(tr("关闭所有(&L)"), this);
    closeAllAct->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_W));
    closeAllAct->setStatusTip(tr("关闭所有文件"));
    connect(closeAllAct, &QAction::triggered, [this]()
    {
        for (int i = tabWidget->count() - 1; i >= 0; --i)
        {
            closeTab(i);
        }
    });

    exitAct = new QAction(tr("退出(&X)"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("退出应用程序"));
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    cutAct = new QAction(tr("剪切(&T)"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("将当前选择的内容剪切到剪贴板"));
    connect(cutAct, &QAction::triggered, [this]() { if (currentEditor()) currentEditor()->cut(); });

    copyAct = new QAction(tr("复制(&C)"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("将当前选择的内容复制到剪贴板"));
    connect(copyAct, &QAction::triggered, [this]() { if (currentEditor()) currentEditor()->copy(); });

    pasteAct = new QAction(tr("粘贴(&P)"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("将剪贴板的内容粘贴到当前选择"));
    connect(pasteAct, &QAction::triggered, [this]() { if (currentEditor()) currentEditor()->paste(); });

    toggleCommentAct = new QAction(tr("切换注释(&/)"), this);
    toggleCommentAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Slash));
    toggleCommentAct->setStatusTip(tr("切换选中行的注释"));
    connect(toggleCommentAct, &QAction::triggered, this, &CompilerIDE::toggleComment);

    findAct = new QAction(tr("查找(&F)..."), this);
    findAct->setShortcuts(QKeySequence::Find);
    findAct->setStatusTip(tr("查找文本"));
    connect(findAct, &QAction::triggered, this, &CompilerIDE::find);

    replaceAct = new QAction(tr("替换(&R)..."), this);
    replaceAct->setShortcuts(QKeySequence::Replace);
    replaceAct->setStatusTip(tr("查找并替换文本"));
    connect(replaceAct, &QAction::triggered, this, &CompilerIDE::replace);

    findNextAct = new QAction(tr("查找下一个"), this);
    findNextAct->setShortcut(QKeySequence(Qt::Key_F3));
    findNextAct->setStatusTip(tr("查找下一个匹配项"));
    connect(findNextAct, &QAction::triggered, this, &CompilerIDE::findNext);

    findPreviousAct = new QAction(tr("查找上一个"), this);
    findPreviousAct->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F3));
    findPreviousAct->setStatusTip(tr("查找上一个匹配项"));
    connect(findPreviousAct, &QAction::triggered, this, &CompilerIDE::findPrevious);

    gotoLineAct = new QAction(tr("转到行(&G)..."), this);
    gotoLineAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_G));
    gotoLineAct->setStatusTip(tr("转到指定行号"));
    connect(gotoLineAct, &QAction::triggered, this, &CompilerIDE::gotoLine);

    debugStartAct = new QAction(tr("开始调试(&D)"), this);
    debugStartAct->setShortcut(QKeySequence(Qt::Key_F5));
    debugStartAct->setStatusTip(tr("开始调试当前程序"));
    connect(debugStartAct, &QAction::triggered, this, [this]()
    {
        if (curFile.isEmpty()) return;
        QString executable = QFileInfo(curFile).absolutePath() + "/" +
                             QFileInfo(curFile).baseName() + ".exe";
        if (QFile::exists(executable))
        {
            debugger->startDebugging(executable, curFile);
        }
        else
        {
            QMessageBox::information(this, tr("调试"), tr("请先编译程序"));
        }
    });

    debugStopAct = new QAction(tr("停止调试"), this);
    debugStopAct->setStatusTip(tr("停止当前调试会话"));
    connect(debugStopAct, &QAction::triggered, debugger, &Debugger::stopDebugging);

    debugContinueAct = new QAction(tr("继续(&C)"), this);
    debugContinueAct->setShortcut(QKeySequence(Qt::Key_F5));
    debugContinueAct->setStatusTip(tr("继续执行"));
    connect(debugContinueAct, &QAction::triggered, debugger, &Debugger::continueExecution);

    debugStepOverAct = new QAction(tr("单步跳过(&O)"), this);
    debugStepOverAct->setShortcut(QKeySequence(Qt::Key_F10));
    debugStepOverAct->setStatusTip(tr("单步跳过当前行"));
    connect(debugStepOverAct, &QAction::triggered, debugger, &Debugger::stepOver);

    debugStepIntoAct = new QAction(tr("单步进入(&I)"), this);
    debugStepIntoAct->setShortcut(QKeySequence(Qt::Key_F11));
    debugStepIntoAct->setStatusTip(tr("单步进入函数"));
    connect(debugStepIntoAct, &QAction::triggered, debugger, &Debugger::stepInto);

    debugStepOutAct = new QAction(tr("单步跳出(&U)"), this);
    debugStepOutAct->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_F11));
    debugStepOutAct->setStatusTip(tr("单步跳出当前函数"));
    connect(debugStepOutAct, &QAction::triggered, debugger, &Debugger::stepOut);

    debugToggleBreakpointAct = new QAction(tr("切换断点(&B)"), this);
    debugToggleBreakpointAct->setShortcut(QKeySequence(Qt::Key_F9));
    debugToggleBreakpointAct->setStatusTip(tr("在当前行切换断点"));
    connect(debugToggleBreakpointAct, &QAction::triggered, this, [this]()
    {
        CodeEditor *editor = currentEditor();
        if (editor)
        {
            QTextCursor cursor = editor->textCursor();
            int line = cursor.blockNumber() + 1;
            // 未实现（应实现断点切换）
        }
    });

    terminalShowAct = new QAction(tr("终端"), this);
    terminalShowAct->setCheckable(true);
    terminalShowAct->setChecked(false);
    terminalShowAct->setStatusTip(tr("显示或隐藏集成终端"));
    connect(terminalShowAct, &QAction::triggered, this, [this](bool checked) {
        terminalDock->setVisible(checked);
    });

    snippetShowAct = new QAction(tr("代码片段"), this);
    snippetShowAct->setCheckable(true);
    snippetShowAct->setChecked(false);
    snippetShowAct->setStatusTip(tr("显示或隐藏代码片段管理器"));
    connect(snippetShowAct, &QAction::triggered, this, [this](bool checked) {
        snippetDock->setVisible(checked);
    });

    loginAct = new QAction(tr("登录(&L)"), this);
    loginAct->setStatusTip(tr("登录您的账户"));
    connect(loginAct, &QAction::triggered, this, &CompilerIDE::showLoginDialog);

    logoutAct = new QAction(tr("登出(&O)"), this);
    logoutAct->setStatusTip(tr("登出当前账户"));
    logoutAct->setEnabled(false);
    connect(logoutAct, &QAction::triggered, this, &CompilerIDE::logout);

    registerAct = new QAction(tr("注册(&R)"), this);
    registerAct->setStatusTip(tr("注册新账户"));
    connect(registerAct, &QAction::triggered, this, &CompilerIDE::showLoginDialog);

    userInfoAct = new QAction(tr("用户信息(&U)"), this);
    userInfoAct->setStatusTip(tr("查看用户信息"));
    userInfoAct->setEnabled(false);
    connect(userInfoAct, &QAction::triggered, this, &CompilerIDE::showUserInfo);

    compileAct = new QAction(tr("编译(&B)"), this);
    compileAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
    compileAct->setStatusTip(tr("编译当前文件"));
    connect(compileAct, &QAction::triggered, this, &CompilerIDE::compile);

    runAct = new QAction(tr("运行(&R)"), this);
    runAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    runAct->setStatusTip(tr("运行上次编译的程序"));
    connect(runAct, &QAction::triggered, this, &CompilerIDE::run);

    compileRunAct = new QAction(tr("编译并运行(&F5)"), this);
    compileRunAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F5));
    compileRunAct->setStatusTip(tr("编译并运行当前文件"));
    connect(compileRunAct, &QAction::triggered, this, &CompilerIDE::compileAndRun);

    clearOutputAct = new QAction(tr("清除输出"), this);
    clearOutputAct->setStatusTip(tr("清除输出面板"));
    connect(clearOutputAct, &QAction::triggered, this, &CompilerIDE::clearOutput);

    settingsAct = new QAction(tr("设置(&S)..."), this);
    settingsAct->setStatusTip(tr("配置应用程序设置"));
    connect(settingsAct, &QAction::triggered, this, &CompilerIDE::showSettings);

    projectManagerAct = new QAction(tr("项目管理器"), this);
    projectManagerAct->setCheckable(true);
    projectManagerAct->setChecked(true);
    projectManagerAct->setStatusTip(tr("显示或隐藏项目管理器"));
    connect(projectManagerAct, &QAction::triggered, this, &CompilerIDE::showProjectManager);

    fileBrowserAct = new QAction(tr("文件浏览器"), this);
    fileBrowserAct->setCheckable(true);
    fileBrowserAct->setChecked(false);
    fileBrowserAct->setStatusTip(tr("显示或隐藏文件浏览器"));
    connect(fileBrowserAct, &QAction::triggered, this, &CompilerIDE::showFileBrowser);

    symbolBrowserAct = new QAction(tr("符号浏览器"), this);
    symbolBrowserAct->setCheckable(true);
    symbolBrowserAct->setChecked(false);
    symbolBrowserAct->setStatusTip(tr("显示或隐藏符号浏览器"));
    connect(symbolBrowserAct, &QAction::triggered, this, &CompilerIDE::showSymbolBrowser);

    outlineAct = new QAction(tr("大纲视图"), this);
    outlineAct->setCheckable(true);
    outlineAct->setChecked(false);
    outlineAct->setStatusTip(tr("显示或隐藏大纲视图"));
    connect(outlineAct, &QAction::triggered, this, &CompilerIDE::showOutline);

    zoomInAct = new QAction(tr("放大"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    zoomInAct->setStatusTip(tr("放大文本"));
    connect(zoomInAct, &QAction::triggered, this, &CompilerIDE::zoomIn);

    zoomOutAct = new QAction(tr("缩小"), this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    zoomOutAct->setStatusTip(tr("缩小文本"));
    connect(zoomOutAct, &QAction::triggered, this, &CompilerIDE::zoomOut);

    resetZoomAct = new QAction(tr("重置缩放"), this);
    resetZoomAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    resetZoomAct->setStatusTip(tr("重置文本缩放"));
    connect(resetZoomAct, &QAction::triggered, this, &CompilerIDE::resetZoom);

    aiCompletionAct = new QAction(tr("AI代码建议"), this);
    aiCompletionAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    aiCompletionAct->setStatusTip(tr("获取AI代码建议"));
    connect(aiCompletionAct, &QAction::triggered, this, &CompilerIDE::showAICompletion);
  
    aboutAct = new QAction(tr("关于(&A)"), this);
    aboutAct->setStatusTip(tr("显示应用程序的关于框"));
    connect(aboutAct, &QAction::triggered, this, &CompilerIDE::about);
    aboutQtAct = new QAction(tr("关于Qt(&Q)"), this);
    aboutQtAct->setStatusTip(tr("显示Qt库的关于框"));
    connect(aboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
    documentationAct = new QAction(tr("文档(&D)"), this);
    documentationAct->setStatusTip(tr("显示C++文档"));
    connect(documentationAct, &QAction::triggered, this, &CompilerIDE::showDocumentation);
    welcomeAct = new QAction(tr("欢迎(&W)"), this);
    welcomeAct->setStatusTip(tr("显示欢迎消息"));
    connect(welcomeAct, &QAction::triggered, this, &CompilerIDE::showWelcomeMessage);
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
}
void CompilerIDE::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("文件(&F)"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(openProjectAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addAction(closeAllAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);
    editMenu = menuBar()->addMenu(tr("编辑(&E)"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    editMenu->addAction(toggleCommentAct);
    editMenu->addSeparator();
    editMenu->addAction(findAct);
    editMenu->addAction(replaceAct);
    editMenu->addAction(findNextAct);
    editMenu->addAction(findPreviousAct);
    editMenu->addAction(gotoLineAct);
    editMenu->addSeparator();
    editMenu->addAction(aiCompletionAct);
    buildMenu = menuBar()->addMenu(tr("构建(&B)"));
    buildMenu->addAction(compileAct);
    buildMenu->addAction(runAct);
    buildMenu->addAction(compileRunAct);
    buildMenu->addSeparator();
    buildMenu->addAction(clearOutputAct);
    QMenu *debugMenu = menuBar()->addMenu(tr("调试(&D)"));
    debugMenu->addAction(debugStartAct);
    debugMenu->addAction(debugStopAct);
    debugMenu->addSeparator();
    debugMenu->addAction(debugContinueAct);
    debugMenu->addAction(debugStepOverAct);
    debugMenu->addAction(debugStepIntoAct);
    debugMenu->addAction(debugStepOutAct);
    debugMenu->addSeparator();
    debugMenu->addAction(debugToggleBreakpointAct);
    viewMenu = menuBar()->addMenu(tr("视图(&V)"));
    viewMenu->addAction(projectManagerAct);
    viewMenu->addAction(fileBrowserAct);
    viewMenu->addAction(symbolBrowserAct);
    viewMenu->addAction(outlineAct);
    viewMenu->addSeparator();
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(resetZoomAct);
    viewMenu->addAction(terminalShowAct);
    viewMenu->addAction(snippetShowAct);
    toolsMenu = menuBar()->addMenu(tr("工具(&T)"));
    toolsMenu->addAction(settingsAct);
    helpMenu = menuBar()->addMenu(tr("帮助(&H)"));
    helpMenu->addAction(welcomeAct);
    helpMenu->addAction(documentationAct);
    helpMenu->addSeparator();
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
    QMenu *accountMenu = menuBar()->addMenu(tr("账户(&A)"));
    accountMenu->addAction(loginAct);
    accountMenu->addAction(registerAct);
    accountMenu->addAction(userInfoAct);
    accountMenu->addSeparator();
    accountMenu->addAction(logoutAct);
}
void CompilerIDE::createToolBars()
{
    fileToolBar = addToolBar(tr("文件"));
    fileToolBar->setObjectName("FileToolBar");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    editToolBar = addToolBar(tr("编辑"));
    editToolBar->setObjectName("EditToolBar");
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(toggleCommentAct);
    editToolBar->addSeparator();
    editToolBar->addAction(findAct);
    editToolBar->addAction(replaceAct);
    buildToolBar = addToolBar(tr("构建"));
    buildToolBar->setObjectName("BuildToolBar");
    buildToolBar->addAction(compileAct);
    buildToolBar->addAction(runAct);
    buildToolBar->addAction(compileRunAct);
    buildToolBar->addAction(clearOutputAct);
    viewToolBar = addToolBar(tr("视图"));
    viewToolBar->setObjectName("ViewToolBar");
    viewToolBar->addAction(zoomInAct);
    viewToolBar->addAction(zoomOutAct);
    viewToolBar->addAction(resetZoomAct);
    buildToolBar->addSeparator();
    QLabel *standardLabel = new QLabel(tr("C++标准:"));
    buildToolBar->addWidget(standardLabel);
    cppStandardCombo = new QComboBox;
    cppStandardCombo->addItem("C++14");
    cppStandardCombo->addItem("C++17");
    cppStandardCombo->addItem("C++20");
    cppStandardCombo->addItem("C++23");
    cppStandardCombo->setCurrentText("C++17");
    connect(cppStandardCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CompilerIDE::changeCppStandard);
    buildToolBar->addWidget(cppStandardCombo);
}
void CompilerIDE::createStatusBar()
{
    statusBar()->showMessage(tr("就绪"));
    userStatusLabel = new QLabel();
    updateUserStatus();
    statusBar()->addPermanentWidget(userStatusLabel);
}
void CompilerIDE::showLoginDialog()
{
    LoginDialog dialog(accountManager, this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QMessageBox::information(this, tr("登录成功"),
                                 tr("欢迎回来，%1！").arg(dialog.getUsername()));
    }
}
void CompilerIDE::logout()
{
    accountManager->logout();
}
void CompilerIDE::showUserInfo()
{
    UserInfoDialog dialog(accountManager, this);
    dialog.exec();
}
void CompilerIDE::updateUserStatus()
{
    if (accountManager->isLoggedIn())
    {
        QString username = accountManager->getCurrentUsername();
        userStatusLabel->setText(tr("用户: %1").arg(username));
        userStatusLabel->setStyleSheet("color: green;");
    }
    else
    {
        userStatusLabel->setText(tr("未登录"));
        userStatusLabel->setStyleSheet("color: red;");
    }
}
void CompilerIDE::onLoginStatusChanged(bool loggedIn)
{
    loginAct->setEnabled(!loggedIn);
    registerAct->setEnabled(!loggedIn);
    logoutAct->setEnabled(loggedIn);
    userInfoAct->setEnabled(loggedIn);
    updateUserStatus();
}
void CompilerIDE::onUserLoggedIn(const QString &username)
{
    Q_UNUSED(username)
    updateUserStatus();
}
void CompilerIDE::createDockWindows()
{
    outputDock = new QDockWidget(tr("输出"), this);
    outputDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    outputEdit = new QPlainTextEdit;
    outputEdit->setReadOnly(true);
    outputEdit->setWordWrapMode(QTextOption::NoWrap);
    QPalette outputPalette = outputEdit->palette();
    outputPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    outputPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    outputEdit->setPalette(outputPalette);
    outputDock->setWidget(outputEdit);
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);
    errorDock = new QDockWidget(tr("错误"), this);
    errorDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    errorListView = new QListView;
    QPalette errorPalette = errorListView->palette();
    errorPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    errorPalette.setColor(QPalette::Text, QColor(220, 220, 220));
    errorListView->setPalette(errorPalette);
    errorListModel = new QStringListModel(this);
    errorListView->setModel(errorListModel);
    connect(errorListView, &QListView::clicked, this, &CompilerIDE::showErrorInCode);
    errorDock->setWidget(errorListView);
    addDockWidget(Qt::BottomDockWidgetArea, errorDock);
    projectManager = new ProjectManager(this);
    projectDock = new QDockWidget(tr("项目管理器"), this);
    projectDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    projectDock->setWidget(projectManager);
    addDockWidget(Qt::LeftDockWidgetArea, projectDock);
    connect(projectManager, &ProjectManager::fileSelected, this, &CompilerIDE::onFileSelected);
    fileBrowserDock = new QDockWidget(tr("文件浏览器"), this);
    fileBrowserDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    fileBrowserDock->setVisible(false);
    addDockWidget(Qt::LeftDockWidgetArea, fileBrowserDock);
    symbolBrowserDock = new QDockWidget(tr("符号浏览器"), this);
    symbolBrowserDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    symbolBrowserDock->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, symbolBrowserDock);
    outlineDock = new QDockWidget(tr("大纲视图"), this);
    outlineDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    outlineDock->setVisible(false);
    addDockWidget(Qt::RightDockWidgetArea, outlineDock);
    if (viewMenu)
    {
        viewMenu->addAction(outputDock->toggleViewAction());
        viewMenu->addAction(errorDock->toggleViewAction());
        viewMenu->addAction(projectDock->toggleViewAction());
        viewMenu->addAction(fileBrowserDock->toggleViewAction());
        viewMenu->addAction(symbolBrowserDock->toggleViewAction());
        viewMenu->addAction(outlineDock->toggleViewAction());
    }
    else
    {
        qWarning() << "viewMenu is not initialized!";
    }
}
void CompilerIDE::createFindDialog()
{
    findDialog = new QDialog(this);
    findDialog->setWindowTitle(tr("查找和替换"));
    findDialog->setModal(false);
    QVBoxLayout *layout = new QVBoxLayout;
    QFormLayout *formLayout = new QFormLayout;
    findEdit = new QLineEdit;
    replaceEdit = new QLineEdit;
    formLayout->addRow(tr("查找:"), findEdit);
    formLayout->addRow(tr("替换为:"), replaceEdit);
    QHBoxLayout *optionsLayout = new QHBoxLayout;
    caseSensitiveCheck = new QCheckBox(tr("区分大小写"));
    wholeWordCheck = new QCheckBox(tr("全字匹配"));
    regexCheck = new QCheckBox(tr("正则表达式"));
    optionsLayout->addWidget(caseSensitiveCheck);
    optionsLayout->addWidget(wholeWordCheck);
    optionsLayout->addWidget(regexCheck);
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    findNextButton = new QPushButton(tr("查找下一个"));
    findPrevButton = new QPushButton(tr("查找上一个"));
    replaceButton = new QPushButton(tr("替换"));
    replaceAllButton = new QPushButton(tr("全部替换"));
    QPushButton *closeButton = new QPushButton(tr("关闭"));
    buttonLayout->addWidget(findNextButton);
    buttonLayout->addWidget(findPrevButton);
    buttonLayout->addWidget(replaceButton);
    buttonLayout->addWidget(replaceAllButton);
    buttonLayout->addWidget(closeButton);
    layout->addLayout(formLayout);
    layout->addLayout(optionsLayout);
    layout->addLayout(buttonLayout);
    findDialog->setLayout(layout);
    connect(findNextButton, &QPushButton::clicked, this, &CompilerIDE::findNext);
    connect(findPrevButton, &QPushButton::clicked, this, &CompilerIDE::findPrevious);
    connect(replaceButton, &QPushButton::clicked, [this]()
    {
        if (currentEditor())
        {
            QTextCursor cursor = currentEditor()->textCursor();
            if (cursor.hasSelection())
            {
                cursor.insertText(replaceEdit->text());
            }
            findNext();
        }
    });
    connect(replaceAllButton, &QPushButton::clicked, [this]()
    {
        if (!currentEditor()) return;
        QTextCursor cursor = currentEditor()->textCursor();
        cursor.movePosition(QTextCursor::Start);
        currentEditor()->setTextCursor(cursor);
        int count = 0;
        while (performFind(true, false))
        {
            cursor = currentEditor()->textCursor();
            cursor.insertText(replaceEdit->text());
            count++;
        }
        QMessageBox::information(this, tr("替换完成"), tr("已替换 %1 个匹配项").arg(count));
    });
    connect(closeButton, &QPushButton::clicked, findDialog, &QDialog::hide);
}
void CompilerIDE::readSettings()
{
    QSettings settings("CompilerIDE", "CompilerIDE 28.3.0");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(1000, 700)).toSize();
    resize(size);
    move(pos);
    compilerPath = settings.value("compilerPath", compilerPath).toString();
    autoBrackets = settings.value("autoBrackets", true).toBool();
    autoQuotes = settings.value("autoQuotes", true).toBool();
    autoIndent = settings.value("autoIndent", true).toBool();
    indentSize = settings.value("indentSize", 4).toInt();
    codeFolding = settings.value("codeFolding", true).toBool();
    lineNumbers = settings.value("lineNumbers", true).toBool();
    darkTheme = settings.value("darkTheme", true).toBool();
    enableAI = settings.value("enableAI", false).toBool();
    aiEndpoint = settings.value("aiEndpoint", "").toString();
    aiKey = settings.value("aiKey", "").toString();
    if (currentEditor())
    {
        currentEditor()->setAutoBracketsEnabled(autoBrackets);
        currentEditor()->setAutoQuotesEnabled(autoQuotes);
        currentEditor()->setAutoIndentEnabled(autoIndent);
        currentEditor()->setIndentSize(indentSize);
        currentEditor()->setCodeFoldingEnabled(codeFolding);
        currentEditor()->setLineNumbersEnabled(lineNumbers);
        currentEditor()->setDarkThemeEnabled(darkTheme);
    }
}
void CompilerIDE::writeSettings()
{
    QSettings settings("CompilerIDE", "CompilerIDE 2.8.6");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
    settings.setValue("compilerPath", compilerPath);
    settings.setValue("autoBrackets", autoBrackets);
    settings.setValue("autoQuotes", autoQuotes);
    settings.setValue("autoIndent", autoIndent);
    settings.setValue("indentSize", indentSize);
    settings.setValue("codeFolding", codeFolding);
    settings.setValue("lineNumbers", lineNumbers);
    settings.setValue("darkTheme", darkTheme);
    settings.setValue("enableAI", enableAI);
    settings.setValue("aiEndpoint", aiEndpoint);
    settings.setValue("aiKey", aiKey);
}
bool CompilerIDE::maybeSave()
{
    CodeEditor *editor = currentEditor();
    if (!editor || !editor->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
        = QMessageBox::warning(this, tr("Compiler IDE"),
                               tr("文档已被修改。\n"
                                  "是否要保存更改？"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret)
    {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}
void CompilerIDE::loadFile(const QString &fileName)
{
    int index = findEditor(fileName);
    if (index >= 0)
   {
        tabWidget->setCurrentIndex(index);
        return;
    }
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Compiler IDE"),
                             tr("无法读取文件 %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }
    QTextStream in(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    in.setCodec("UTF-8");
#else
    in.setEncoding(QStringConverter::Utf8);
#endif
    QApplication::setOverrideCursor(Qt::WaitCursor);
    CodeEditor *editor = new CodeEditor;
    editor->setPlainText(in.readAll());
    editor->setAutoBracketsEnabled(autoBrackets);
    editor->setAutoQuotesEnabled(autoQuotes);
    editor->setAutoIndentEnabled(autoIndent);
    editor->setIndentSize(indentSize);
    editor->setCodeFoldingEnabled(codeFolding);
    editor->setLineNumbersEnabled(lineNumbers);
    editor->setDarkThemeEnabled(darkTheme);
    new CppHighlighter(editor->document());
    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &CompilerIDE::documentWasModified);
    index = tabWidget->addTab(editor, strippedName(fileName));
    tabWidget->setCurrentIndex(index);
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    addToRecentFiles(fileName);
    statusBar()->showMessage(tr("文件已加载"), 2000);
}
bool CompilerIDE::saveFile(const QString &fileName)
{
    CodeEditor *editor = currentEditor();
    if (!editor) return false;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("Compiler IDE"),
                             tr("无法写入文件 %1:\n%2.")
                                 .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return false;
    }

    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");
#else
    out.setEncoding(QStringConverter::Utf8);
#endif
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << editor->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("文件已保存"), 2000);
    return true;
}
void CompilerIDE::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    CodeEditor *editor = currentEditor();
    if (editor)
    {
        editor->document()->setModified(false);
    }
    setWindowModified(false);
    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.cpp";
    setWindowFilePath(shownName);
    int index = tabWidget->currentIndex();
    if (index >= 0)
    {
        tabWidget->setTabText(index, strippedName(shownName));
    }
}
QString CompilerIDE::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
void CompilerIDE::newFile()
{
    CodeEditor *editor = new CodeEditor;
    editor->setAutoBracketsEnabled(autoBrackets);
    editor->setAutoQuotesEnabled(autoQuotes);
    editor->setAutoIndentEnabled(autoIndent);
    editor->setIndentSize(indentSize);
    editor->setCodeFoldingEnabled(codeFolding);
    editor->setLineNumbersEnabled(lineNumbers);
    editor->setDarkThemeEnabled(darkTheme);
    new CppHighlighter(editor->document());
    connect(editor->document(), &QTextDocument::contentsChanged,
            this, &CompilerIDE::documentWasModified);
    int index = tabWidget->addTab(editor, tr("untitled.cpp"));
    tabWidget->setCurrentIndex(index);
    setCurrentFile("");
}
void CompilerIDE::open()
{
    if (maybeSave())
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), "",
                                                        tr("C++文件 (*.cpp *.cc *.cxx *.c++ *.h *.hpp *.hh *.hxx *.h++);;所有文件 (*)"));
        if (!fileName.isEmpty())
        {
            loadFile(fileName);
        }
    }
}
void CompilerIDE::openProject()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("打开项目"), "",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
    {
        projectManager->setRootPath(dir);
        projectDock->setWindowTitle(tr("项目管理器 - %1").arg(QDir(dir).dirName()));
    }
}
bool CompilerIDE::save()
{
    if (curFile.isEmpty())
    {
        return saveAs();
    }
    else
    {
        return saveFile(curFile);
    }
}
bool CompilerIDE::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), "",
                                                    tr("C++文件 (*.cpp *.cc *.cxx *.c++ *.h *.hpp *.hh *.hxx *.h++);;所有文件 (*)"));
    if (fileName.isEmpty())
    {
        return false;
    }
    if (!fileName.contains('.'))
    {
        fileName += ".cpp";
    }
    return saveFile(fileName);
}
void CompilerIDE::about()
{
    QMessageBox::about(this, tr("关于 Compiler IDE"),
                       tr("<h2>Compiler IDE 2.8.6</h2>"
                          "<p>使用Qt 6.9.2构建的现代C++ IDE</p>"
                          "<p>新特性:</p>"
                          "<ul>"
                          "<li>多文件标签页编辑</li>"
                          "<li>项目管理器</li>"
                          "<li>代码折叠</li>"
                          "<li>高级查找和替换</li>"
                          "<li>符号浏览器和大纲视图</li>"
                          "<li>缩放功能</li>"
                          "<li>AI代码建议集成</li>"
                          "<li>改进的语法高亮</li>"
                          "<li>增强的错误检测和报告</li>"
                          "<li>支持C++14、C++17、C++20和C++23标准</li>"
                          "<li>完全支持bits/stdc++.h头文件</li>"
                          "<li>生成.exe文件并直接运行</li>"
                          "</ul>"
                          "<p>Copyright © 2025 Compiler IDE Team</p>"));
}
void CompilerIDE::documentWasModified()
{
    CodeEditor *editor = currentEditor();
    if (editor)
    {
        setWindowModified(editor->document()->isModified());
        int index = tabWidget->currentIndex();
        if (index >= 0)
        {
            QString title = tabWidget->tabText(index);
            if (title.endsWith("*"))
            {
                if (!editor->document()->isModified())
                {
                    title.chop(1);
                    tabWidget->setTabText(index, title);
                }
            }
            else
            {
                if (editor->document()->isModified())
                {
                    tabWidget->setTabText(index, title + "*");
                }
            }
        }
    }
    updateStatusBar();
}
void CompilerIDE::compile()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    if (curFile.isEmpty() && !save())
    {
        return;
    }
    if (editor->document()->isModified())
    {
        if (!save())
        {
            return;
        }
    }
    outputEdit->clear();
    errorListModel->setStringList(QStringList());
    outputEdit->appendPlainText(tr("编译中..."));
    QStringList args;
    if (compilerType == "msvc")
    {
        if (cppStandard == "c++14")
        {
            args << "/std:c++14";
        } else if (cppStandard == "c++17")
        {
            args << "/std:c++17";
        } else if (cppStandard == "c++20")
        {
            args << "/std:c++20";
        } else if (cppStandard == "c++23")
        {
            args << "/std:c++latest";
        }
        args << "/EHsc" << "/W4" << "/nologo";
        args << "/I.";
        args << "/utf-8";
        QString outputFile = QFileInfo(curFile).absolutePath() + "/" + QFileInfo(curFile).baseName() + ".exe";
        args << "/Fe" + outputFile;
        args << curFile;

    }
    else
    {
        if (cppStandard == "c++14")
        {
            args << "-std=c++14";
        } else if (cppStandard == "c++17")
        {
            args << "-std=c++17";
        } else if (cppStandard == "c++20")
        {
            args << "-std=c++20";
        } else if (cppStandard == "c++23")
        {
            args << "-std=c++2b";
        }
        args << "-Wall" << "-Wextra" << "-pedantic";
        args << "-finput-charset=UTF-8" << "-fexec-charset=UTF-8";
        QString outputFile = QFileInfo(curFile).absolutePath() + "/" + QFileInfo(curFile).baseName();
#ifdef Q_OS_WIN
        outputFile += ".exe";
#endif
        args << "-o" << outputFile;
        args << curFile;
    }
    if (compileProcess)
    {
        compileProcess->kill();
        compileProcess->waitForFinished();
        delete compileProcess;
    }
    compileProcess = new QProcess(this);
    compileProcess->setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_WIN
    env.insert("PYTHONIOENCODING", "utf-8");
    env.insert("UTF-8", "1");
#endif
    compileProcess->setProcessEnvironment(env);
    connect(compileProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CompilerIDE::compilerFinished);
    connect(compileProcess, &QProcess::readyReadStandardOutput,
            this, &CompilerIDE::readCompilerOutput);
    connect(compileProcess, &QProcess::readyReadStandardError,
            this, &CompilerIDE::readCompilerError);
    compileProcess->start(compilerPath, args);
    if (!compileProcess->waitForStarted()) {
        outputEdit->appendPlainText(tr("无法启动编译器: %1").arg(compilerPath));
        if (compilerType == "msvc") {
            outputEdit->appendPlainText(tr("注意: MSVC编译器可能需要从开发者命令提示符运行"));
        }
    }
}
void CompilerIDE::runExecutable(const QString &programPath)
{
    outputEdit->appendPlainText(tr("运行程序..."));
    outputEdit->appendPlainText("");
    QString command = "cmd.exe /c \"chcp 65001 >nul && \"" + programPath + "\" & echo. & echo 程序已退出，代码为 %errorlevel% & pause\"";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    wchar_t commandW[2048];
    command.toWCharArray(commandW);
    commandW[command.length()] = '\0';
    if (CreateProcess(NULL, commandW, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        outputEdit->appendPlainText(tr("已弹出控制台窗口运行..."));
        outputEdit->appendPlainText(tr("注意: 控制台已设置为UTF-8编码以支持中文显示"));
    }
    else
    {
        DWORD err = GetLastError();
        outputEdit->appendPlainText(tr("无法运行程序: %1 (错误代码: %2)").arg(programPath).arg(err));
        QProcess *process = new QProcess(this);
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("PYTHONIOENCODING", "utf-8");
#ifdef Q_OS_WIN
        env.insert("CHCP", "65001");
#endif
        process->setProcessEnvironment(env);

        if (process->startDetached(programPath))
        {
            outputEdit->appendPlainText(tr("已通过备用方法启动程序"));
        }
        else
        {
            outputEdit->appendPlainText(tr("备用启动方法失败"));
        }
        delete process;
    }
}
void CompilerIDE::run()
{
    QString program = QFileInfo(curFile).baseName();
#ifdef Q_OS_WIN
    program += ".exe";
#endif
    QString programPath = QFileInfo(curFile).absolutePath() + "/" + program;
    if (!QFile::exists(programPath))
    {
        outputEdit->appendPlainText(tr("未找到可执行文件。先编译..."));
        compileAndRun();
        return;
    }

    runExecutable(programPath);
}
void CompilerIDE::compileAndRun()
{
    outputEdit->clear();
    errorListModel->setStringList(QStringList());
    connect(this, &CompilerIDE::compilationFinished, this, [this](bool success) {
        if (success)
        {
            QTimer::singleShot(100, this, &CompilerIDE::run);
        } else
        {
            outputEdit->appendPlainText(tr("编译失败。无法运行程序。"));
        }
        disconnect(this, &CompilerIDE::compilationFinished, nullptr, nullptr);
    });
    compile();
}
void CompilerIDE::clearOutput()
{
    outputEdit->clear();
    errorListModel->setStringList(QStringList());
}
void CompilerIDE::compilerFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    outputDock->show();
    outputDock->raise();
    outputEdit->verticalScrollBar()->setValue(outputEdit->verticalScrollBar()->maximum());
    if (exitCode == 0)
    {
        outputEdit->appendPlainText(tr("编译成功!"));
        emit compilationFinished(true);
    } else
    {
        outputEdit->appendPlainText(tr("编译失败，退出代码 %1").arg(exitCode));
        emit compilationFinished(false);
    }
}
void CompilerIDE::readCompilerOutput()
{
    QString output = QString::fromLocal8Bit(compileProcess->readAllStandardOutput());
    parseCompilerOutput(output);
}
void CompilerIDE::readCompilerError()
{
    QString errorOutput = QString::fromLocal8Bit(compileProcess->readAllStandardError());
    parseCompilerOutput(errorOutput);
}
void CompilerIDE::parseCompilerOutput(const QString &output)
{
    if (output.trimmed().isEmpty()) return;
    QStringList errors;
    QStringList warnings;
    QStringList lines = output.split("\n", Qt::SkipEmptyParts);
    for (int i = 0; i < lines.size(); i++)
    {
        QString line = lines[i].trimmed();
        if (line.isEmpty()) continue;
        bool isError = false;
        bool isWarning = false;
        QString errorMsg;
        QRegularExpression msvcPattern(R"((.+)\((\d+)\)\s*:\s*(error|warning)\s+(\w+\d+):\s*(.+))");
        QRegularExpressionMatch msvcMatch = msvcPattern.match(line);
        if (msvcMatch.hasMatch())
        {
            QString filename = msvcMatch.captured(1);
            int lineNumber = msvcMatch.captured(2).toInt();
            QString type = msvcMatch.captured(3);
            QString errorCode = msvcMatch.captured(4);
            QString message = msvcMatch.captured(5);
            if (type == "error")
            {
                isError = true;
            }
            else if (type == "warning")
            {
                isWarning = true;
            }
            if (QFileInfo(filename).fileName() == QFileInfo(curFile).fileName())
            {
                errorMsg = QString(tr("第 %1 行: [%2 %3] %4")).arg(lineNumber).arg(type).arg(errorCode).arg(message);
            }
            else
            {
                errorMsg = QString(tr("%1:%2: [%3 %4] %5")).arg(filename).arg(lineNumber).arg(type).arg(errorCode).arg(message);
            }
        }
        if (errorMsg.isEmpty())
        {
            QRegularExpression gccPattern(R"((.+):(\d+):(\d+):\s*(error|warning):\s*(.+))");
            QRegularExpressionMatch gccMatch = gccPattern.match(line);
            if (gccMatch.hasMatch())
            {
                QString filename = gccMatch.captured(1);
                int lineNumber = gccMatch.captured(2).toInt();
                int columnNumber = gccMatch.captured(3).toInt();
                QString type = gccMatch.captured(4);
                QString message = gccMatch.captured(5);
                if (type == "error")
                {
                    isError = true;
                }
                else if (type == "warning")
                {
                    isWarning = true;
                }
                if (QFileInfo(filename).fileName() == QFileInfo(curFile).fileName())
                {
                    errorMsg = QString(tr("第 %1 行, 第 %2 列: [%3] %4")).arg(lineNumber).arg(columnNumber).arg(type).arg(message);
                }
                else
                {
                    errorMsg = QString(tr("%1:%2:%3: [%4] %5")).arg(filename).arg(lineNumber).arg(columnNumber).arg(type).arg(message);
                }
            }
        }
        if (errorMsg.isEmpty())
        {
            QRegularExpression gccSimplePattern(R"((.+):(\d+):\s*(error|warning):\s*(.+))");
            QRegularExpressionMatch gccSimpleMatch = gccSimplePattern.match(line);
            if (gccSimpleMatch.hasMatch())
            {
                QString filename = gccSimpleMatch.captured(1);
                int lineNumber = gccSimpleMatch.captured(2).toInt();
                QString type = gccSimpleMatch.captured(3);
                QString message = gccSimpleMatch.captured(4);
                if (type == "error")
                {
                    isError = true;
                }
                else if (type == "warning")
                {
                    isWarning = true;
                }
                if (QFileInfo(filename).fileName() == QFileInfo(curFile).fileName())
                {
                    errorMsg = QString(tr("第 %1 行: [%2] %3")).arg(lineNumber).arg(type).arg(message);
                }
                else
                {
                    errorMsg = QString(tr("%1:%2: [%3] %4")).arg(filename).arg(lineNumber).arg(type).arg(message);
                }
            }
        }
        if (errorMsg.isEmpty())
        {
            if (line.contains("error", Qt::CaseInsensitive) || line.contains("错误", Qt::CaseInsensitive))
            {
                errorMsg = line;
                isError = true;
            }
            else if (line.contains("warning", Qt::CaseInsensitive) || line.contains("警告", Qt::CaseInsensitive))
            {
                errorMsg = line;
                isWarning = true;
            }
        }
        if (!errorMsg.isEmpty())
        {
            errorMsg += "\n" + QString(50, '_');
            if (isError)
            {
                errors << errorMsg;
            }
            else if (isWarning)
            {
                warnings << errorMsg;
            } else
            {
                errors << errorMsg;
            }
        }
        if (i + 1 < lines.size() && errorMsg.isEmpty())
        {
            QString nextLine = lines[i + 1].trimmed();
            if (nextLine.contains("error", Qt::CaseInsensitive) ||
                nextLine.contains("warning", Qt::CaseInsensitive))
            {
                QString combinedMsg = line + "\n" + nextLine;
                combinedMsg += "\n" + QString(50, '_');
                if (nextLine.contains("error", Qt::CaseInsensitive))
                {
                    errors << combinedMsg;
                } else {
                    warnings << combinedMsg;
                }
                i++;
            }
        }
    }
    QStringList allMessages;
    allMessages.append(errors);
    allMessages.append(warnings);
    errorListModel->setStringList(allMessages);
    if (!allMessages.isEmpty())
    {
        errorDock->show();
        errorDock->raise();
    }
}
void CompilerIDE::runProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus)
    outputEdit->appendPlainText("\n");
    outputEdit->appendPlainText(tr("'%1' (进程 %2) 已退出，代码为 %3 .")
                                    .arg(runProcess->program())
                                    .arg(runProcess->processId())
                                    .arg(exitCode));
    outputEdit->appendPlainText(tr("按任意键关闭此窗口. . ."));
}
void CompilerIDE::readRunOutput()
{
    QString output = QString::fromLocal8Bit(runProcess->readAllStandardOutput());
    QString error = QString::fromLocal8Bit(runProcess->readAllStandardError());
    if (!output.isEmpty())
    {
        outputEdit->appendPlainText(output);
    }
    if (!error.isEmpty())
    {
        outputEdit->appendPlainText(error);
    }
}
void CompilerIDE::showErrorInCode(const QModelIndex &index)
{
    QString error = errorListModel->data(index, Qt::DisplayRole).toString();
    QRegularExpression linePattern(R"(第\s+(\d+)\s+行)");
    QRegularExpressionMatch match = linePattern.match(error);
    if (match.hasMatch()) {
        int lineNumber = match.captured(1).toInt();
        CodeEditor *editor = currentEditor();
        if (editor) {
            QTextBlock block = editor->document()->findBlockByLineNumber(lineNumber - 1);
            if (block.isValid())
            {
                QTextCursor cursor(block);
                editor->setTextCursor(cursor);
                editor->centerCursor();
            }
        }
    }
}
void CompilerIDE::updateStatusBar()
{
    CodeEditor *editor = currentEditor();
    if (!editor)
    {
        statusBar()->showMessage(tr("就绪"));
        return;
    }
    if (curFile.isEmpty())
    {
        statusBar()->showMessage(tr("无标题 - 第 %1 行, 第 %2 列 | 已修改 | %3")
                                     .arg(editor->textCursor().blockNumber() + 1)
                                     .arg(editor->textCursor().columnNumber() + 1)
                                     .arg(cppStandard));
    }
    else
    {
        statusBar()->showMessage(tr("%1 - 第 %2 行, 第 %3 列 | %4 | %5")
                                     .arg(strippedName(curFile))
                                     .arg(editor->textCursor().blockNumber() + 1)
                                     .arg(editor->textCursor().columnNumber() + 1)
                                     .arg(cppStandard)
                                     .arg(editor->document()->isModified() ? tr("已修改") : tr("已保存")));
    }
}
void CompilerIDE::changeCppStandard(int index)
{
    QString standard = cppStandardCombo->itemText(index).toLower();
    cppStandard = standard;
    updateStatusBar();
}
void CompilerIDE::showSettings()
{
    SettingsDialog dialog(this, compilerPath, autoBrackets, autoQuotes, autoIndent, indentSize, codeFolding, lineNumbers, darkTheme, enableAI, aiEndpoint, aiKey);
    if (dialog.exec() == QDialog::Accepted)
    {
        applySettings(dialog.getCompilerPath(), dialog.getAutoBrackets(),
                      dialog.getAutoQuotes(), dialog.getAutoIndent(), dialog.getIndentSize(),
                      dialog.getCodeFolding(), dialog.getLineNumbers(), dialog.getDarkTheme(),
                      dialog.getEnableAI(), dialog.getAIEndpoint(), dialog.getAIKey());
    }
}
void CompilerIDE::applySettings(const QString &compilerPath, bool autoBrackets,
                                bool autoQuotes, bool autoIndent, int indentSize,
                                bool codeFolding, bool lineNumbers, bool darkTheme,
                                bool enableAI, const QString &aiEndpoint, const QString &aiKey)
{
    this->compilerPath = compilerPath;
    this->autoBrackets = autoBrackets;
    this->autoQuotes = autoQuotes;
    this->autoIndent = autoIndent;
    this->indentSize = indentSize;
    this->codeFolding = codeFolding;
    this->lineNumbers = lineNumbers;
    if (this->darkTheme != darkTheme)
    {
        this->darkTheme = darkTheme;
        setTheme(darkTheme);
    }
    this->enableAI = enableAI;
    this->aiEndpoint = aiEndpoint;
    this->aiKey = aiKey;
    this->compilerType = detectCompilerType(compilerPath);
    for (int i = 0; i < tabWidget->count(); ++i) 
   {
        CodeEditor *editor = editorAt(i);
        if (editor) {
            editor->setAutoBracketsEnabled(autoBrackets);
            editor->setAutoQuotesEnabled(autoQuotes);
            editor->setAutoIndentEnabled(autoIndent);
            editor->setIndentSize(indentSize);
            editor->setCodeFoldingEnabled(codeFolding);
            editor->setLineNumbersEnabled(lineNumbers);
        }
    }
    writeSettings();
    updateStatusBar();
}
void CompilerIDE::toggleComment()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QTextCursor cursor = editor->textCursor();
    if (!cursor.hasSelection())
    {
        cursor.select(QTextCursor::LineUnderCursor);
    }
    QString selectedText = cursor.selectedText();
    QStringList lines = selectedText.split(QChar::ParagraphSeparator);
    bool allCommented = true;
    for (const QString &line : lines)
    {
        if (!line.trimmed().startsWith("//") && !line.trimmed().isEmpty())
        {
            allCommented = false;
            break;
        }
    }
    for (int i = 0; i < lines.size(); ++i)
    {
        if (allCommented)
        {
            if (lines[i].trimmed().startsWith("//"))
            {
                int commentPos = lines[i].indexOf("//");
                lines[i].remove(commentPos, 2);
            }
        }
        else
        {
            if (!lines[i].trimmed().isEmpty() && !lines[i].trimmed().startsWith("//"))
            {
                int firstNonSpace = 0;
                while (firstNonSpace < lines[i].length() && lines[i][firstNonSpace].isSpace())
                {
                    firstNonSpace++;
                }
                lines[i].insert(firstNonSpace, "//");
            }
        }
    }
    cursor.insertText(lines.join(QChar::ParagraphSeparator));
}
void CompilerIDE::showDocumentation()
{
    QDesktopServices::openUrl(QUrl("https://en.cppreference.com/w/"));
}
void CompilerIDE::showWelcomeMessage()
{
    outputEdit->setPlainText(tr("欢迎使用 Compiler IDE 2.8.6!\n\n")
                             + tr("此版本的新特性:\n")
                             + tr("• 多文件标签页编辑\n")
                             + tr("• 项目管理器\n")
                             + tr("• 代码折叠\n")
                             + tr("• 高级查找和替换\n")
                             + tr("• 符号浏览器和大纲视图\n")
                             + tr("• 缩放功能\n")
                             + tr("• AI代码建议集成\n")
                             + tr("• 改进的语法高亮\n")
                             + tr("• 拥有启动和欢迎界面\n")
                             + tr("• 生成.exe文件并直接运行\n\n")
                             + tr("若要开始使用，请创建新文件或打开现有文件。"));
}
void CompilerIDE::onFileSelected(const QString &filePath)
{
    loadFile(filePath);
}
void CompilerIDE::closeTab(int index)
{
    CodeEditor *editor = editorAt(index);
    if (editor && editor->document()->isModified())
    {
        tabWidget->setCurrentIndex(index);
        if (!maybeSave())
        {
            return;
        }
    }
    tabWidget->removeTab(index);
    delete editor;
    if (tabWidget->count() == 0)
    {
        setCurrentFile("");
    }
}
void CompilerIDE::currentTabChanged(int index)
{
    if (index < 0)
    {
        setCurrentFile("");
        return;
    }
    CodeEditor *editor = editorAt(index);
    if (editor)
    {
        editor->setCompleter(codeCompleter);
        editor->setCompletionEnabled(true);
        QString fileName = tabWidget->tabText(index);
        if (fileName.endsWith("*"))
        {
            fileName.chop(1);
        }
        for (int i = 0; i < tabWidget->count(); ++i) 
        {
            CodeEditor *ed = editorAt(i);
            if (ed == editor)
            {
                curFile = fileName;
                break;
            }
        }

        updateStatusBar();
    }
}
CodeEditor* CompilerIDE::currentEditor() const
{
    return qobject_cast<CodeEditor*>(tabWidget->currentWidget());
}
CodeEditor* CompilerIDE::editorAt(int index) const
{
    return qobject_cast<CodeEditor*>(tabWidget->widget(index));
}
int CompilerIDE::findEditor(const QString &fileName) const
{
    for (int i = 0; i < tabWidget->count(); ++i)
    {
        QString tabText = tabWidget->tabText(i);
        if (tabText.endsWith("*"))
        {
            tabText.chop(1);
        }
        if (tabText == QFileInfo(fileName).fileName())
        {
            return i;
        }
    }
    return -1;
}
void CompilerIDE::find()
{
    updateFindDialog();
    findDialog->show();
    findDialog->raise();
    findEdit->setFocus();
    findEdit->selectAll();
}
void CompilerIDE::replace()
{
    updateFindDialog();
    findDialog->show();
    findDialog->raise();
    findEdit->setFocus();
    findEdit->selectAll();
}
void CompilerIDE::findNext()
{
    performFind(true);
}
void CompilerIDE::findPrevious()
{
    performFind(false);
}
void CompilerIDE::gotoLine()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    bool ok;
    int line = QInputDialog::getInt(this, tr("转到行"), tr("行号:"),
                                    1, 1, editor->document()->blockCount(), 1, &ok);
    if (ok)
    {
        QTextBlock block = editor->document()->findBlockByLineNumber(line - 1);
        if (block.isValid())
        {
            QTextCursor cursor(block);
            editor->setTextCursor(cursor);
            editor->centerCursor();
        }
    }
}
void CompilerIDE::showProjectManager()
{
    projectDock->setVisible(projectManagerAct->isChecked());
}
void CompilerIDE::showFileBrowser()
{
    fileBrowserDock->setVisible(fileBrowserAct->isChecked());
}
void CompilerIDE::showSymbolBrowser()
{
    symbolBrowserDock->setVisible(symbolBrowserAct->isChecked());
}
void CompilerIDE::showOutline()
{
    outlineDock->setVisible(outlineAct->isChecked());
}
void CompilerIDE::zoomIn()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QFont font = editor->font();
    font.setPointSize(font.pointSize() + 1);
    editor->setFont(font);
}
void CompilerIDE::zoomOut()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QFont font = editor->font();
    font.setPointSize(font.pointSize() - 1);
    editor->setFont(font);
}
void CompilerIDE::resetZoom()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QFont font = editor->font();
    font.setPointSize(11);
    editor->setFont(font);
}
void CompilerIDE::showAICompletion()
{
    if (!enableAI)
    {
        QMessageBox::information(this, tr("AI功能未启用"),
                                 tr("请在设置中启用AI代码建议功能并配置API端点。"));
        return;
    }
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QTextCursor cursor = editor->textCursor();
    cursor.select(QTextCursor::LineUnderCursor);
    QString lineText = cursor.selectedText();
    // 简化处理
    QMessageBox::information(this, tr("AI代码建议"),
                             tr("AI代码建议功能正在开发中。\n当前行: %1").arg(lineText));
}

void CompilerIDE::updateFindDialog()
{
    CodeEditor *editor = currentEditor();
    if (!editor) return;
    QTextCursor cursor = editor->textCursor();
    if (cursor.hasSelection())
    {
        findEdit->setText(cursor.selectedText());
    }
}
bool CompilerIDE::performFind(bool forward, bool showMessage)
{
    CodeEditor *editor = currentEditor();
    if (!editor) return false;
    QString searchText = findEdit->text();
    if (searchText.isEmpty()) return false;
    QTextDocument::FindFlags flags;
    if (!forward) flags |= QTextDocument::FindBackward;
    if (caseSensitiveCheck->isChecked()) flags |= QTextDocument::FindCaseSensitively;
    if (wholeWordCheck->isChecked()) flags |= QTextDocument::FindWholeWords;
    bool found = false;
    if (regexCheck->isChecked())
    {
        QRegularExpression regex(searchText);
        if (caseSensitiveCheck->isChecked())
        {
            regex.setPatternOptions(QRegularExpression::NoPatternOption);
        }
        else
        {
            regex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        }
        found = editor->find(regex, flags);
    }
    else
    {
        found = editor->find(searchText, flags);
    }
    if (!found && showMessage)
    {
        QMessageBox::information(this, tr("查找"), tr("找不到\"%1\"").arg(searchText));
    }
    return found;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("CompilerIDE");
    QApplication::setApplicationName("Compiler IDE 2.8.6");
    QApplication::setApplicationVersion("2.8.6");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif
    SplashScreen *splash = new SplashScreen();
    splash->showWithAnimation();
    CompilerIDE *ide = new CompilerIDE();
    QObject::connect(splash, &SplashScreen::hidden, [splash, ide, argc, argv]()
    {
        splash->deleteLater();
        WelcomeDialog welcomeDialog;
        if (welcomeDialog.exec() == QDialog::Accepted)
        {
            QString selectedFile = welcomeDialog.getSelectedFile();
            ide->show();
            if (!selectedFile.isEmpty())
            {
                ide->loadFile(selectedFile);
            }
            else if (argc > 1)
            {
                ide->loadFile(argv[1]);
            }
        }
        else
        {
            ide->deleteLater();
            QApplication::quit();
        }
    });
    int result = app.exec();
    delete ide;
    return result;
}

#include "main.moc"
