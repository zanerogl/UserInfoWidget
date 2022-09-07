#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    initProfile();
    initUI();

    readUserData();
    loadUserInfo(m_userName, m_password);
    connect(m_creatBtn, &QPushButton::clicked, this, &Widget::addUser);

    connect(m_saveBtn, &QPushButton::clicked, this, &Widget::on_saveBtnisClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &Widget::on_cancelBtnisClicked);

}

Widget::~Widget()
{
    delete ui;
//    deleteButtons();
//    if(m_tableWidget!=NULL)
//    {
//        delete m_tableWidget;
//        m_tableWidget=nullptr;
//    }
}

void Widget::initProfile()
{
    m_userInfo = new QSettings(m_filePath, QSettings::IniFormat);
    userCount = m_userInfo->childGroups().size(); //用户数量 == 组数 == 键数/每组键数
}

void Widget::initUI()
{
    m_tableWidget = new QTableWidget(this);
    m_creatBtn = new QPushButton(this);
    m_saveBtn = new QPushButton(this);
    m_cancelBtn = new QPushButton(this);
    m_gridLayout = new QGridLayout(this);

    /*设置布局*/
    QSpacerItem *verticalSpacer = new QSpacerItem(20, 60, QSizePolicy::Minimum, QSizePolicy::Fixed);
    m_gridLayout->addWidget(m_tableWidget, 0, 0, 1, 4);
    m_gridLayout->addWidget(m_creatBtn, 1, 1, 2, 2);    //位于第一行第一列，占两行两列是因为点击后出现的两个按钮需要占两列
    m_gridLayout->addWidget(m_saveBtn, 1, 1, 2, 1);
    m_gridLayout->addWidget(m_cancelBtn, 1, 2, 2, 1);
    m_gridLayout->addItem(verticalSpacer, 2, 1, 1, 1);
    m_gridLayout->setMargin(3);

    /*设置TableWidget属性*/
    m_tableWidget->setColumnCount(m_headerStr.size());
    m_tableWidget->setHorizontalHeaderLabels(m_headerStr);
    QFont font = m_tableWidget->horizontalHeader()->font();
    font.setBold(true); //加粗
    font.setPixelSize(16);
    m_tableWidget->horizontalHeader()->setFont(font);

    m_tableWidget->setColumnWidth(0, 225);  //用户名
    m_tableWidget->setColumnWidth(1, 350);  //密码
    m_tableWidget->setColumnWidth(3, 145);  //选项

    m_tableWidget->setShowGrid(true);
//    m_tableWidget->setGridStyle(Qt::DotLine);   //点线网格
//    m_tableWidget->setSortingEnabled(true);     //排序
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);//最后一行可拉伸
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setStyleSheet("QTableWidget{ background-color: #c0c0c0; alternate-background-color: #606060; selection-background-color:#282828;}");
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setTextElideMode(Qt::ElideRight);

    /*设置按钮属性*/
    m_creatBtn->setText("New Account");
    m_creatBtn->setStyleSheet("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}");
    m_saveBtn->setText("Save");
    m_saveBtn->setStyleSheet("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}");
    m_saveBtn->hide();
    m_cancelBtn->setText("Cancel");
    m_cancelBtn->setStyleSheet("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}");
    m_cancelBtn->hide();
}

void Widget::loadUserInfo(QList<QString> userName, QList<QString> userPassword)
{
    m_tableWidget->clearContents();
    if(userName.size() != userPassword.size())
    {
        return;
    }
    m_tableWidget->setRowCount(userCount);      //设置行数，用于测试

    for(int i = 0; i<m_tableWidget->rowCount(); i++)
    {
        /*用户名*/
        QTableWidgetItem *nameItem = new QTableWidgetItem(tr("%1").arg(userName.at(i)));
        nameItem->setFlags(nameItem->flags() & (~Qt::ItemIsEditable));//第一列不可编辑
        nameItem->setTextAlignment(Qt::AlignCenter);
        m_tableWidget->setItem(i, 0, nameItem);

        /*密码*/
        QTableWidgetItem *pwdItem = new QTableWidgetItem(tr("%1").arg(userPassword.at(i)));
        pwdItem->setFlags(pwdItem->flags() & (~Qt::ItemIsEditable));//第二列不可编辑
        pwdItem->setTextAlignment(Qt::AlignCenter);
        m_tableWidget->setItem(i, 1, pwdItem);

        /*添加删除按钮和修改按钮*/
        QPushButton *btnDel = new QPushButton("Delete");
        btnDel->setStyleSheet(QString::fromUtf8("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}"));
        QPushButton *btnUpdate = new QPushButton("Modify");
        btnUpdate->setStyleSheet(QString::fromUtf8("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}"));

        btnVec.push_back(btnDel);
        btnVec.push_back(btnUpdate);

        QWidget *widget = new QWidget;
        QHBoxLayout *hboxlayout = new QHBoxLayout(widget);
        hboxlayout->setMargin(2);
        hboxlayout->addWidget(btnDel);
        hboxlayout->addWidget(btnUpdate);
        m_tableWidget->setCellWidget(i, 2,  widget);

        connect(btnDel, &QPushButton::clicked, [=](){this->removeUser(nameItem);});
        connect(btnUpdate, &QPushButton::clicked, [=](){this->modifyUser(btnUpdate, nameItem);});
    }
}

void Widget::deleteButtons()
{
//    qDebug()<<__FUNCTION__;
    /*释放table_user表格中动态申请的QPushButton*/
    int len = btnVec.length();
    for(int i = 0; i<len; i++)
    {
        QPushButton *btnPtr = btnVec.at(i);
        if(btnPtr != nullptr)
        {
            delete btnPtr;
            btnPtr = nullptr;
        }
    }
}

void Widget::removeUser(QTableWidgetItem *item)
{
    if(QMessageBox::question(this, "Tip","Are you sure you want to delete?", QMessageBox::Yes|QMessageBox::No)== QMessageBox::Yes)
    {
        userCount--;
        int row = m_tableWidget->row(item);
        QString user = m_tableWidget->item(row, 0)->text();
        m_userInfo->remove(user + "/" + "Password");
        m_tableWidget->removeRow(row);
    }
}

void Widget::modifyUser(QPushButton *btn, QTableWidgetItem *item)
{
    int row = m_tableWidget->row(item);
    Qt::ItemFlags nameFlag = m_tableWidget->item(row, 0)->flags();
    Qt::ItemFlags pwdFlag = m_tableWidget->item(row, 1)->flags();

    if(btn->text() == "Modify")
    {
        btn->setText("Save");
        m_tableWidget->item(row, 0)->setFlags(nameFlag | (Qt::ItemIsEditable));     //用户名->可编辑
        m_tableWidget->item(row, 1)->setFlags(pwdFlag | (Qt::ItemIsEditable));      //密码->可编辑
        m_creatBtn->setEnabled(false);      //禁用新用户按钮
    }
    else if(!m_userName.contains(m_tableWidget->item(row, 0)->text()))  //不存在重复用户名
    {
        btn->setText("Modify");
        m_tableWidget->item(row, 0)->setFlags(nameFlag & (~Qt::ItemIsEditable));    //用户名->可编辑
        m_tableWidget->item(row, 1)->setFlags(pwdFlag & (~Qt::ItemIsEditable));     //密码->不可编辑
        m_creatBtn->setEnabled(true);      //禁用新用户按钮
        m_userInfo->setValue(m_tableWidget->item(row, 0)->text() + "/" + "Password", m_tableWidget->item(row, 1)->text());
        readUserData();                    //更新m_userName 和 m_password
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Username already exists.", QMessageBox::Ok);
    }
}

void Widget::addUser()
{
    m_tableWidget->setRowCount(m_tableWidget->rowCount()+1);

    int row = m_tableWidget->rowCount()-1;
    /*用户名*/
    QTableWidgetItem *nameItem = new QTableWidgetItem(tr("%1").arg("username"+QString::number(m_tableWidget->rowCount()-1)));
    nameItem->setTextAlignment(Qt::AlignCenter);
    m_tableWidget->setItem(row, 0, nameItem);

    /*密码*/
    QTableWidgetItem *pwdItem = new QTableWidgetItem(tr("%1").arg("password"));
    pwdItem->setTextAlignment(Qt::AlignCenter);
    m_tableWidget->setItem(row, 1, pwdItem);

    /*按钮*/
    QTableWidgetItem *btnItem = new QTableWidgetItem(tr("%1").arg(""));
    btnItem->setFlags(btnItem->flags() & (~Qt::ItemIsEditable));//不可编辑
    m_tableWidget->setItem(row, 2, btnItem);

    /*隐藏creatBtn*/
    m_creatBtn->hide();
    m_saveBtn->show();
    m_cancelBtn->show();
}

void Widget::on_saveBtnisClicked()
{
    int row = m_tableWidget->rowCount()-1;      //获取最后一行的编号

    QTableWidgetItem *nameItem = m_tableWidget->item(row, 0);
    QTableWidgetItem *pwdItem = m_tableWidget->item(row, 1);

    if(!m_userName.contains(m_tableWidget->item(row, 0)->text()))
    {
        m_saveBtn->hide();
        m_cancelBtn->hide();
        m_creatBtn->show();

        /*添加删除按钮和修改按钮*/
        QPushButton *btnDel = new QPushButton("Delete");
        btnDel->setEnabled(true);
        btnDel->setStyleSheet(QString::fromUtf8("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}"));
        QPushButton *btnUpdate = new QPushButton("Modify");
        btnUpdate->setEnabled(true);
        btnUpdate->setStyleSheet(QString::fromUtf8("QPushButton{background: white; color: blacke; border: 2px solid black; border-radius:10px; padding:2px 4px;}"));
        QWidget *widget = new QWidget;
        QHBoxLayout *hboxlayout = new QHBoxLayout(widget);
        hboxlayout->setMargin(2);
        hboxlayout->addWidget(btnDel);
        hboxlayout->addWidget(btnUpdate);
        m_tableWidget->setCellWidget(m_tableWidget->rowCount()-1, 2,  widget);

        nameItem->setFlags(m_tableWidget->item(row, 0)->flags() & (~Qt::ItemIsEditable));  //第一列不可编辑
        pwdItem->setFlags(pwdItem->flags() & (~Qt::ItemIsEditable));    //第二列不可编辑

        connect(btnDel, &QPushButton::clicked, [=](){ this->removeUser(nameItem); });
        connect(btnUpdate, &QPushButton::clicked, [=](){ this->modifyUser(btnUpdate, nameItem); });

        m_userInfo->setValue(nameItem->text() + "/" + "Password", pwdItem->text());
        userCount++;
    }
    else
    {
        QMessageBox::warning(this, "Warning", "Username already exists.", QMessageBox::Ok);
    }
}

void Widget::on_cancelBtnisClicked()
{
    m_saveBtn->hide();
    m_cancelBtn->hide();
    m_creatBtn->show();
    m_tableWidget->removeRow(m_tableWidget->rowCount()-1);  //删除未添加信息的行
}

void Widget::readUserData()
{
    m_userName.clear();
    m_password.clear();

    m_userName = m_userInfo->childGroups(); //组名对应用户名
    for(int userIndex = 0; userIndex < m_userName.size(); userIndex++)
    {
        m_password<<m_userInfo->value(m_userName.at(userIndex) + "/" + "Password", 1024).toString();
    }
}

