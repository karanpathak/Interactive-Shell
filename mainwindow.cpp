#include "mainwindow.h"
#include "dialog.h"
#include "ui_mainwindow.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define size 1024

QString bash= "$kp" ;



QString cwd()
{
   char buf[size];
   getcwd(buf,sizeof(buf));
   return (QString ::fromStdString(buf));

}
QString inp;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString sho;
    sho.append(bash);chdir("/");
    ui->setupUi(this);connect(ui->lineEdit, SIGNAL(returnPressed()),ui->pushButton,SIGNAL(clicked()));
    ui->textBrowser->setTextColor( QColor("white"));


    QPalette* palette = new QPalette();
    palette->setColor(QPalette::Base,Qt::black);
    ui->textBrowser->setPalette(*palette);

    sho.append(cwd());sho.append("#");
    ui->textBrowser->append(sho);

}

//FOR DETERMINING mode_t
mode_t MainWindow::find_mode(char* para)
{
int i,mode[4]={0},mode_oth=0000,mode_g=0000,mode_u=0000,mode_id=0000;
for(i=0;i<strlen(para);mode[3-i]=para[strlen(para)-1-i]-'0',i++);

//mode bit for other
if (mode[3]!=0)
{ switch(mode[3])
   { case 1 : mode_oth=mode_oth|S_IXOTH;break;// execute
     case 2 : mode_oth=mode_oth|S_IWOTH;break;// write
     case 4 : mode_oth=mode_oth|S_IROTH;break;// read
     case 3 : mode_oth=mode_oth|S_IWOTH|S_IXOTH;break;// write + execute
     case 5 : mode_oth=mode_oth|S_IROTH|S_IXOTH;break;// read + execute
     case 6 : mode_oth=mode_oth|S_IWOTH|S_IROTH;break;// write + read
     case 7 : mode_oth=mode_oth|S_IRWXO;break;// write + execute + read
     default : ui->textBrowser->append("\nwrong other bit... treating it as 0\n");
    }

}

//MODE BIT FOR GROUP
if (mode[2]!=0)
{ switch(mode[2])
   { case 1:mode_g=mode_g|S_IXGRP;break;// execute
     case 2:mode_g=mode_g|S_IWGRP;break;// write
     case 4:mode_g=mode_g|S_IRGRP;break;// read
     case 3:mode_g=mode_g|S_IWGRP|S_IXGRP;break;// write + execute
     case 5:mode_g=mode_g|S_IRGRP|S_IXGRP;break;// read + execute
     case 6:mode_g=mode_g|S_IWGRP|S_IRGRP;break;// write + read
     case 7:mode_g=mode_g|S_IRWXG;break;// write + execute + read
     default : ui->textBrowser->append("\nwrong group bit... treating it as 0\n");
   }
}
if (mode[1]!=0)
{ switch(mode[1])
   { case 1:mode_u=mode_u|S_IXUSR;break;// execute
     case 2:mode_u=mode_u|S_IWUSR;break;// write
     case 4:mode_u=mode_u|S_IRUSR;break;// read
     case 3:mode_u=mode_u|S_IWUSR|S_IXUSR;break;// write + execute
     case 5:mode_u=mode_u|S_IRUSR|S_IXUSR;break;// read + execute
     case 6:mode_u=mode_u|S_IWUSR|S_IRUSR;break;// write + read
     case 7:mode_u=mode_u|S_IRWXU;break;// write + execute + read
     default : ui->textBrowser->append("\nwrong owner bit... treating it as 0\n");
   }
}
if (mode[0]!=0)
{ switch(mode[0])
   { case 1:mode_id=mode_id|S_ISVTX ;break;//sticky bit
     case 2:mode_id=mode_id|S_ISGID;break;//set group ID on execution
     case 4:mode_id=mode_id|S_ISUID ;break;//set user id on exectution
     default : ui->textBrowser->append("wrong highest  bit... treating it as 0\n");
  }

}

return (mode_oth|mode_g|mode_u|mode_id);

}




void MainWindow::cpy(char *src,char *dest,option opt[5],int o)		        // o denotes number of options
{
   int ifd,ofd,i=0;
   int op[26]={0};
   struct stat z;
   char buf[size],c;
   mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // read and write privelege modes for users,groups, and others
   int oflag =O_WRONLY|O_CREAT,iflag=O_RDONLY;				            // flags to signify open for which operation
   ssize_t si;
   i=0;
   if(o!=0)
   {
      i=0;
      while(i<o)
      {
         ofd=(opt[i].mode)-'a';
         if((ofd==5)&&(op[8]==1||op[13]==1))			           //to resolve conflicts according to priority
         {
         }
         else if(ofd==8)
         {
             op[8]=1;
             op[13]=0;
             op[5]=0;
         }
         else if(ofd==13)
         {
            op[13]=1;
            op[8]=0;
            op[5]=0;
         }
         else
         {
            op[ofd]=1;
         }
         ++i;
      }
      i=0;
      if(op[7]==1)
      {
         ui->textBrowser->append("+a, archives or preserves all attributes\n");
         ui->textBrowser->append("+b makes backup of given path file in current directory\n");
         ui->textBrowser->append("+f, if an existing destination file cannot be opened, remove it and try again(redundant if the +n option is used)\n");
         ui->textBrowser->append("+i, prompts before overwrite (overrides a previous +n option) \n");
         ui->textBrowser->append("+p same as +a, preserves mode,ownership,timestamps\n");
         ui->textBrowser->append("+n, do not overwrite an existing file (overrides a previous +i option) \n");
         return;
      }
      // +b
      if(op[1]==1)
      {
         std::string cw;char temp[100];
         char *te;
         QString s=cwd();
         cw=(s.toStdString());


         te=src;

         while(*(te+1)!='\0')
         {
            ++te;
         }
         while(*te!='/')
         {
            --te;
         }
         int j;



         //strcpy
         for(j=0;cw[j]!='\0';temp[j]=cw[j],j++);ofd=j;




         while(*te!='\0')
         {
            temp[ofd]=*te; //append in current working directory
            ++te;
            ++ofd;
         }
         temp[ofd]='\0';
         if(dest==NULL)
         {printf("karan");
            dest=temp;
         }
         else   // backing in current working directory
         {
            ifd=open(src,iflag);
            ofd=open(temp,oflag,mode);
            while((si=read(ifd,buf,size-1))>0)				//reading bytes in to the buffer
            {
                if(write(ofd,buf,si)!=si)				//writing bytes from buffer to file ,and checking for errors
                {
                   break;
                }
            }
            ::close(ifd);
            ::close(ofd);
         }
      }
      //+f
      if(op[5]==1)
      {
         if(dest==NULL)
         {
           ui->textBrowser->append("Destination cannot be null for +f\n");
           return;
         }
         else if((ofd=open(dest,O_WRONLY,mode))==-1)
         {
            unlink(dest);
         }
         if(ofd!=-1)
         {
            ::close(ofd);
            ofd=0;
         }
      }
      // +i or +n
      if(op[13]==1||op[8]==1)
      {
         if(dest==NULL)
         {
           ui->textBrowser->append("Destination cannot be null for +n or +i\n");
           return;
         }
         else if((ofd=open(dest,O_WRONLY|O_CREAT|O_EXCL,mode))==-1)
         {
            if(op[8]==1)
            {
               ui->textBrowser->append("Overwrite existing file ?(y/n) : \n");
               scanf("%c",&c);
               if(c=='y'||c=='Y')
               {
               }
               else
               {
                  return;
               }
            }
            else
            {
               ui->textBrowser->append("File not over-written!\n");
               return;
            }
         }
         if(ofd!=-1)
         {
            ::close(ofd);
            ofd=0;
         }
      }
   }
   ifd=open(src,iflag);                                                         //By default opened with all reading priveleges
   ofd=open(dest,oflag,mode);
   if(ifd==-1||ofd==-1)							       //file descripror not properly assigned
   {
      ui->textBrowser->append("\nFailure to copy!\n");
      return;
   }
   // wrtiting in destination
   while((si=read(ifd,buf,size-1))>0)						//reading bytes in to the buffer
   {
       if(write(ofd,buf,si)!=si)						//writing bytes from buffer to file,and checking for errors
       {
          break;
       }
   }
   if(si==-1)
   {
      ui->textBrowser->append("Error while reading\n");
   }
   if(op[0]==1||op[15]==1)
   {
      fstat(ifd,&z);
      fchmod(ofd,z.st_mode);
      fchown(ofd,z.st_uid,z.st_gid);
   }
   ::close(ifd);
   ::close(ofd);
}

void MainWindow::mve(char *src, char *dest,option opt[5],int o)
{
   cpy(src,dest,opt,o);
   if(unlink(src)==-1)								//unlink and erase if not open
   {
      ui->textBrowser->append("Error! The file is still open in another process!\n");
   }

}

//	MAKE DIRECTORY
void MainWindow::mdir(const char* pathname,option* opt , int n)
{ int _m=-1,_p=-1,_h=0;
// knowing the option
for(int i=0;i<n;i++)
{
if(opt[i].mode=='p' || opt[i].mode=='P'){_p=i;}
else if(opt[i].mode=='m'|| opt[i].mode=='M'){_m=i;}
else if(opt[i].mode=='h' || opt[i].mode=='H'){_h=1;}
}
// m & p modes
if(_m!=-1 && _p!=-1)
{
if(!mkdir(opt[_p].parameter,find_mode(opt[_m].parameter)))
{ QString temp;temp.append("Directory ");temp.append(QString ::fromStdString(opt[_p].parameter));temp.append(" ");temp.append("created successfully\n");
    ui->textBrowser->append(temp);return ;
}
else
{
    QString temp;temp.append("Error");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
    ui->textBrowser->append(temp); return ;
}
}
// p mode
if(_p!=-1)
{if(!mkdir(opt[_p].parameter,0777))
{QString temp;temp.append("Directory ");temp.append(QString ::fromStdString(opt[_p].parameter));temp.append(" ");temp.append("created successfully\n");
        ui->textBrowser->append(temp);return ;
}
else
    {
        QString temp;temp.append("Error");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
        ui->textBrowser->append(temp);return ;}
}
// m mode
if(_m!=-1)
{if(!mkdir(pathname,find_mode(opt[_m].parameter)))
{QString temp;temp.append("Directory ");temp.append(QString ::fromStdString(pathname));temp.append(" ");temp.append("created successfully\n");
        ui->textBrowser->append(temp);return ;}
else
{ QString temp;temp.append("Error");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
        ui->textBrowser->append(temp);return ;}
}
// help mode
if(_h==1)
{
ui->textBrowser->append("Used to make a new directory\nusage:\nmdir directory_path  +options {p,m,h} option_parameter\nm=mode (e.g.)mdir /test +m 0755\np=parent (e.g.) mdir +p /test\n\n");
return ;}
//  no mode
if(_m==-1)
{if(!mkdir(pathname,0777))
    {
        QString temp;temp.append("Directory ");temp.append(QString ::fromStdString(pathname));temp.append(" ");temp.append("created successfully\n");
                ui->textBrowser->append(temp);
    }
else
    {QString temp;temp.append("Error");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
        ui->textBrowser->append(temp);}
}

}

///CHANGING MODES
void MainWindow::chp(const char* pathname,char* val1,option* opt,int n)
{
if(n!=0)
{
if(opt[0].mode=='h' || opt[0].mode=='H')
{
    ui->textBrowser->append("Specifies/changes file permission\nusage:\nchp path file_permission\n");
ui->textBrowser->append("File permission :\n1=execute , 2=write , 4=read\n(e.g) chp /test 752\nHere 7=owner 5=group 2=other\n");
}
}

else
{if(!chmod(pathname,find_mode(val1))){ui->textBrowser->append("Permission changed successfully\n");}
else
{QString temp;temp.append("Error");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
ui->textBrowser->append(temp);
}
}
}


//   CHANGE DIRECTORY
void MainWindow::cdir(char* pathname,option* opt,int n)
{
if(n!=0)
{
if(opt[n-1].mode=='h' ||opt[n-1].mode=='H')
ui->textBrowser->append("Used to change directory\nusage:\ncdir pathname\n\n");
}
else
{
if(!strcmp(pathname,".."))
{
int i;
char buf[size];
  getcwd(buf,sizeof(buf));
  for(i=strlen(buf)-1;buf[i]!='/' && i>=0;pathname[size-i]=buf[i],i--);
}
if(!chdir(pathname))
 { QString temp;
temp.append("directory changed to :");temp.append("  ");temp.append(cwd());temp.append("\n");
ui->textBrowser->append(temp);
}
 else
 { QString temp;temp.append("Error:");temp.append(QString::fromStdString(strerror( errno)));temp.append("\n");
    ui->textBrowser->append(temp);

 }
}
}

/// parser
void MainWindow::init_parser(const char* input)
{
int i,j,k;
char fn[10],value1[15],value2[15];
option opt[5];

//FOR CATCHING FUNCTION
for(i=0;input[i]!=' ' && input[i]!='\0';fn[i]=input[i],i++);fn[i]='\0';

while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces

// FOR CATCHING value1
if(input[i]!='\0')
{
for(k=0;input[i]!=' ' && input[i]!='+' && input[i]!='\0';value1[k]=input[i],k++,i++);value1[k]='\0';

}

// FOR CATCHING value2
while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces

if(input[i]!='\0')
{ // VALUE 2
if(input[i]!='+')
{
for(k=0;input[i]!=' ' && input[i]!='+' && input[i]!='\0';value2[k]=input[i],k++,i++);value2[k]='\0';
}
}

// CATCHING OPTIONS
while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces

{ j=0;
while(input[i]!='\0' && input[i]=='+')
  { i++;
    while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces
    // CATCHING MODES
    if(input[i]!='\0')
     {
       opt[j].mode=input[i];i++;
     }
   //// CATCHING PARAMETER
   while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces
   if(input[i]!='\0')
    {
      if(input[i]!='+')
        {for(k=0;input[i]!=' ' && input[i]!='+' && input[i]!='\0';opt[j].parameter[k]=input[i],k++,i++);opt[j].parameter[k]='\0';}
    }
    while(input[i] == ' ' && input[i]!='\0'){i++;}//excluding white spaces
   j++;
  }

}//end of catching options

if(!strcmp(fn,"cwd"))
{QString temp;temp.append("Currently in:");temp.append("  ");temp.append(cwd());temp.append("\n");
    ui->textBrowser->append(temp);//return 1;
}
else if(!strcmp(fn,"cdir"))
{MainWindow::cdir(value1,opt,j);//return 1;
}
else if(!strcmp(fn,"mdir"))
{mdir(value1,opt,j);//return 1;
}
else if(!strcmp(fn,"chp"))
{chp(value1,value2,opt,j);//return 1;
}
else if(!strcmp(fn,"cpy"))
{
    if(value2[0]=='\0')
    {
       cpy(value1,NULL,opt,j);
    }
    else
    {
       cpy(value1,value2,opt,j);
    }
}
else if(!strcmp(fn,"mve"))
mve(value1,value2,opt,j);

else if(!strcmp(fn,"help"))
 { ui->textBrowser->append("\ntry:\ncwd : Shows current working directory\nclear : Clears the Screen \nmdir +h\ncdir +h\nchp +h\n\nTo Know More...\n\n");
//return 1;
}
else if(!strcmp(fn,"exit"))
{ exit(0);

}
else if(!strcmp(fn,"clear"))
    ui->textBrowser->setText("");
else
{ ui->textBrowser->append("Command not recorgnized\n");
    //return 1;
}


}//end of parser



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{ QString sho;
  sho.append(bash);

   inp=ui->lineEdit->text();   // takes the input
       ui->lineEdit->setText("");

      init_parser(inp.toStdString().c_str());
       sho.append(cwd());sho.append("#");
       ui->textBrowser->append(sho);


}

void MainWindow::on_pushButton_2_clicked()
{   init_parser("cwd");
    QString sho;
      sho.append(bash);
      sho.append(cwd());sho.append("#");
      ui->textBrowser->append(sho);

}


void MainWindow::on_pushButton_3_clicked()
{
    Dialog di;
    di.setModal(true);di.exec();
    QString directory=di.directory();
    QString folder_name=di.folder_name();
    QString pathname;pathname.append(directory); pathname.append("/");pathname.append(folder_name);
    QString command;command.append("mdir ");command.append(pathname);
    init_parser(command.toStdString().c_str());
    //ui->textBrowser->append(directory);
    //ui->textBrowser->append(folder_name);
    QString sho;
      sho.append(bash);
      sho.append(cwd());sho.append("#");
      ui->textBrowser->append(sho);

}
