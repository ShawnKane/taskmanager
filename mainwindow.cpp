#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    class WorkerThread : public QThread {
    public:
        WorkerThread(MainWindow* mainWindow) : mainWindow(mainWindow) {}

        void run() override {
            while (true) {
                mainWindow->coutProcess(); // Виклик методу через посилання на об'єкт
                QApplication::processEvents();
                QThread::msleep(1000);// Затримка 1 секунда (1000 мілісекунд)
            }
        }

    private:
        MainWindow* mainWindow;
    };

    WorkerThread* workerThread = new WorkerThread(this);
    workerThread->start();
}

void MainWindow::coutProcess()
{



    // Створення об'єкту процесів
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed." << std::endl;
    }

    // Ініціалізація структури для інформації про процес
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Отримання першого процесу
    if (!Process32First(hProcessSnap, &pe32)) {
        std::cerr << "Process32First failed." << std::endl;
        CloseHandle(hProcessSnap);
    }
    int rowCount = 0;
    std::vector<int> vectorPID;
    // Виведення інформації про всі процеси
    do {



        // Відкриття процесу для додаткової інформації
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
        if (hProcess != nullptr) {

            int memory;
            // Отримання інформації про використання пам'яті
            PROCESS_MEMORY_COUNTERS_EX pmc;
            if (GetProcessMemoryInfo(hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                memory = pmc.PrivateUsage / 1024 / 1024;
            }

            // Отримання інформації про використання процесора
            FILETIME createTime, exitTime, kernelTime, userTime;
            double elapsedTimeInSeconds;
            if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime)) {
                ULARGE_INTEGER createTimeU;
                createTimeU.LowPart = createTime.dwLowDateTime;
                createTimeU.HighPart = createTime.dwHighDateTime;

                SYSTEMTIME startTime;
                FileTimeToSystemTime(&createTime, &startTime);

                SYSTEMTIME currentTime;
                GetSystemTime(&currentTime);

                ULONGLONG elapsedTime = 0;
                SystemTimeToFileTime(&currentTime, &exitTime);
                ULARGE_INTEGER exitTimeU;
                exitTimeU.LowPart = exitTime.dwLowDateTime;
                exitTimeU.HighPart = exitTime.dwHighDateTime;

                elapsedTime = exitTimeU.QuadPart - createTimeU.QuadPart;
                elapsedTimeInSeconds = static_cast<double>(elapsedTime) / 10000000.0;

                std::cout << "Elapsed Time since Process Start: " << elapsedTimeInSeconds << " seconds" << std::endl;
            }




            QString path;
            // Отримання повного шляху до процесу
            wchar_t fullPath[MAX_PATH];
            DWORD fullPathSize = MAX_PATH;
            if (QueryFullProcessImageName(hProcess, 0, fullPath, &fullPathSize)) {

                path  = QString::fromWCharArray(fullPath);
            }


            int PID = pe32.th32ProcessID;
            QString exeName = QString::fromWCharArray(pe32.szExeFile);

            //int rowCount = ui->tableWidget->rowCount();
            bool find = false;
            for(int elem : vectorPID)
            {


                if(PID == elem)
                {
                    find = true;
                    std::cout<<"find"<<std::endl;
                    break;
                }
            }
            if(!find)
            {
                vectorPID.push_back(PID);
                ui->tableWidget->removeRow(rowCount);

                ui->tableWidget->insertRow(rowCount);

                //std::cout<<rowCount<<std::endl;
                for(int j = 0; j < ui->tableWidget->columnCount(); j++)
                {
                    QString totaltime;
                    QTableWidgetItem *tbl = nullptr;;
                    switch(j)
                    {
                    case 0:
                        tbl = new QTableWidgetItem(QString(exeName));
                        break;

                    case 1:
                        tbl = new QTableWidgetItem(QString::number(PID));
                        break;
                    case 2:
                        totaltime = QString::number(elapsedTimeInSeconds, 'f', 0);
                        tbl = new QTableWidgetItem(QString(totaltime));
                        break;
                    case 3:
                        tbl = new QTableWidgetItem(QString::number(memory));
                        break;
                    case 4:
                        tbl = new QTableWidgetItem(QString(path));
                        break;
                    }
                    if(tbl)
                    {

                        ui->tableWidget->setItem(rowCount,j,tbl);

                    }


                }
                rowCount++;
            }


            CloseHandle(hProcess);



        }
    } while (Process32Next(hProcessSnap, &pe32));
    std::cout<<"reset!"<<std::endl;
    //ui->tableWidget->setRowCount(ui->tableWidget->rowCount()-1);
    CloseHandle(hProcessSnap);
}

MainWindow::~MainWindow()
{
    delete ui;
}

