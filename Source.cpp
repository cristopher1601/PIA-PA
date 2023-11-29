#include <windows.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include "resource.h"
#include <time.h>
#include <commctrl.h>
#include <ctime>
#include <sstream>
#include <vector>

using namespace std;

char mascota[12][50] = { "Perro","Gato","Hamster","Gallo","Caballo","Cuyo","Conejo","Puerquito","Pato","Raton","Alcon","Changuito" };
char ao[7][30] = { "Nombre del cliente", "Teléfono", "Nombre de la mascota", "Especie de la mascota", "Motivo de consulta", "Costo de la consulta", "Fecha" };
int indi = 0, intcount = 0;

const int MAX_TEXT_LENGTH = 100;

struct Veterinario
{
    char nombre[100];
    char cedula[50];
    char clave[50];
    char contraseña[50];
    char path_foto[260];
    Veterinario* sig;
    Veterinario* ant;
};

struct Citas
{
    char NombrCliente[50];
    char TelCliente[12];
    char Mascota[50];
    char NameMascota[50];
    char MotivoConsulta[500];
    char Costo[5];
    SYSTEMTIME fecha;


    Citas* sig;
    Citas* ant;
};

char nombreUsuario[100];
char cedula[50];
char clave[50];
char contraseña[50];
char path_foto[260];

Citas* lista;
Citas* Inicio = NULL;

Veterinario* inicio = NULL;     // Inicializar la lista de veterinarios como vacía
Veterinario* aux = NULL;        //variable auxiliar

LRESULT CALLBACK Login(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CrearCuenta(HWND hwndCrearCuenta, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Menu(HWND hwndMenu, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Alta(HWND hwndAlta, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Baja(HWND hwndBaja, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Modificar(HWND hwndModificar, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK Agenda(HWND hwndAgenda, UINT msg, WPARAM wParam, LPARAM lParam);

//hay que agregar un void de carga de archivos y en la linea 
void AgregarVeterinario(char nombre[], char cedula[], char usuario[], char contraseña[], char path_foto[]);
void Capturarveterinarios();
void capturarcitas();
void Datos_Cargar();
void Datos_Guardar();
void Datos_Veterinario_Cargar();
void Datos_Veterinario_Guardar();
bool existeCitaEnFecha(SYSTEMTIME nuevaFecha);

int CompareDates(const SYSTEMTIME& date1, const SYSTEMTIME& date2)
{
    if (date1.wYear != date2.wYear)
    {
        return date1.wYear - date2.wYear;
    }
    else if (date1.wMonth != date2.wMonth)
    {
        return date1.wMonth - date2.wMonth;
    }
    else
    {
        return date1.wDay - date2.wDay;
    }
}

HWND hwnd = NULL;
HWND hwndLogin = NULL;
HWND hwndCrearCuenta = NULL;
HWND hwndMenu = NULL;
HWND hwndAlta = NULL;
HWND hwndBaja = NULL;
HWND hwndModificar = NULL;
HWND hwndAgenda = NULL;
HWND hwndListBox = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{

    SYSTEMTIME sysTimeActual;
    GetLocalTime(&sysTimeActual);

    // Leer datos al iniciar la aplicación
    Datos_Cargar();
    Datos_Veterinario_Cargar();


    MSG mensaje;
    hwnd = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, Login);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    while (TRUE == GetMessage(&mensaje, 0, 0, 0))
    {
        TranslateMessage(&mensaje);
        DispatchMessage(&mensaje);
    }
    return mensaje.wParam;

}

LRESULT CALLBACK Login(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {

    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON2: // Botón de inicio de sesión
        {
            GetDlgItemText(hwnd, IDC_EDIT2, nombreUsuario, sizeof(nombreUsuario));
            GetDlgItemText(hwnd, IDC_EDIT1, contraseña, sizeof(contraseña));

            bool encontrado = false;
            Veterinario* actual = inicio;
            while (actual != NULL)
            {
                if (strcmp(actual->nombre, nombreUsuario) == 0 && strcmp(actual->contraseña, contraseña) == 0)
                {
                    encontrado = true;
                    break;
                }
                actual = actual->sig;
            }
            if (encontrado)
            {
                MessageBox(hwnd, "Inicio de sesión exitoso", "Éxito", MB_OK);
                hwndMenu = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG2), hwndMenu, Menu);
                ShowWindow(hwnd, SW_HIDE);
                ShowWindow(hwndMenu, SW_SHOWDEFAULT);
            }
            else
            {
                MessageBox(hwnd, "Inicio de sesión fallido. Verifica tus credenciales.", "Error", MB_OK | MB_ICONERROR);
            }
        }
        break;

        case IDC_BUTTON1:
        {
            if (hwndCrearCuenta == NULL)
            {
                hwndCrearCuenta = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG3), hwnd, CrearCuenta);
                ShowWindow(hwnd, SW_HIDE);
                ShowWindow(hwndCrearCuenta, SW_SHOW);
                SetDlgItemText(hwnd, IDC_EDIT1, "");
                SetDlgItemText(hwnd, IDC_EDIT2, "");
            }
            hwndCrearCuenta = NULL;
        }
        break;
        }
    }
    break;

    case WM_CLOSE:

        Datos_Veterinario_Guardar();
        Datos_Guardar();
    {
        DestroyWindow(hwnd);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;
    }
    return 0;
}

LRESULT CALLBACK CrearCuenta(HWND hwndCrearCuenta, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        // Inicialización de la ventana IDD_DIALOG3
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON1: // Cargar una foto
        {
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(OPENFILENAME));

            char cDirFile[MAX_PATH] = "";

            ofn.hwndOwner = hwndCrearCuenta;
            ofn.lpstrFile = cDirFile;
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrDefExt = "bmp";
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            ofn.lpstrFilter = "Imágenes BMP\0*.bmp\0";

            if (GetOpenFileName(&ofn)) {
                HWND hPControl = GetDlgItem(hwndCrearCuenta, IDC_STATIC1); // IDC_IMAGEN es el ID del control de imagen
                HBITMAP hImagen = (HBITMAP)LoadImage(NULL, cDirFile, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                if (hImagen != NULL) {
                    SendMessage(hPControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagen);
                    strcpy_s(path_foto, cDirFile);
                }
                else {
                    MessageBox(hwndCrearCuenta, "No se pudo cargar la imagen.", "Error", MB_OK | MB_ICONERROR);
                }
            }
            else {
                DWORD dwError = CommDlgExtendedError();
                if (dwError != 0) {
                    MessageBox(hwndCrearCuenta, "Error al abrir el cuadro de diálogo de archivos.", "Error", MB_OK | MB_ICONERROR);
                }

            }
        }
        break;

        case IDC_BUTTON2: // Botón para crear la nueva cuenta
        {
            // Obtener los datos ingresados en los campos de texto
            GetDlgItemText(hwndCrearCuenta, IDC_EDIT1, nombreUsuario, sizeof(nombreUsuario));
            GetDlgItemText(hwndCrearCuenta, IDC_EDIT2, cedula, sizeof(cedula));
            GetDlgItemText(hwndCrearCuenta, IDC_EDIT3, clave, sizeof(clave));
            GetDlgItemText(hwndCrearCuenta, IDC_EDIT4, contraseña, sizeof(contraseña));

            if (strlen(nombreUsuario) == 0 || strlen(contraseña) == 0 || strlen(cedula) == 0 || strlen(clave) == 0 || strlen(path_foto) == 0)
            {
                MessageBox(hwndCrearCuenta, "Por favor, complete todos los campos.", "Error", MB_OK | MB_ICONERROR);
            }
            else
            {
                //Mandar a llamar a todas los parametros, incluido el de la foto
                AgregarVeterinario(nombreUsuario, cedula, clave, contraseña, path_foto);
                MessageBox(hwndCrearCuenta, "Nueva cuenta creada con éxito", "Éxito", MB_OK);
                EndDialog(hwndCrearCuenta, IDOK);
                ShowWindow(hwnd, SW_SHOW);
                SetDlgItemText(hwnd, IDC_EDIT1, "");
                SetDlgItemText(hwnd, IDC_EDIT2, "");
                SetDlgItemText(hwnd, IDC_EDIT3, "");
                SetDlgItemText(hwnd, IDC_EDIT4, "");
            }
        }
        break;

        case IDC_BUTTON3: // Botón para salir
        {
            int response = MessageBox(hwndCrearCuenta, "¿Seguro que quiere salir?", "Aviso", MB_YESNO | MB_ICONWARNING);
            if (response == IDYES)
            {
                ShowWindow(hwndCrearCuenta, SW_HIDE);
                ShowWindow(hwnd, SW_SHOWDEFAULT);
                SetDlgItemText(hwnd, IDC_EDIT1, "");
                SetDlgItemText(hwnd, IDC_EDIT2, "");
                SetDlgItemText(hwnd, IDC_EDIT3, "");
                SetDlgItemText(hwnd, IDC_EDIT4, "");
            }
        }
        break;

        // Otros casos para más botones u operaciones...

        }
        break;

    case WM_CLOSE:
        DestroyWindow(hwndCrearCuenta);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return 0;
}

LRESULT CALLBACK Menu(HWND hwndMenu, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        // Nombre del doctor

        bool encontrado = false;
        Veterinario* actual = inicio;
        while (actual != NULL)
        {
            if (strcmp(actual->nombre, nombreUsuario) == 0)
            {
                encontrado = true;
                break;
            }
            actual = actual->sig;
        }
        if (encontrado)
        {
            SetDlgItemText(hwndMenu, IDC_MEDICONOMBRE, actual->nombre);

            // Cargar imagen del doctor
            Veterinario* vetActual = inicio;
            while (vetActual != NULL) {
                if (vetActual == actual) {  // Validar si es el veterinario correcto
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, vetActual->path_foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                    SendDlgItemMessage(hwndMenu, 1099, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    break;  // Una vez encontrado el veterinario, sal del bucle
                }
                vetActual = vetActual->sig;
            }
        }

    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_SALIRMENU:
        {
            int Opc = MessageBox(hwndMenu, "¿Seguro desea salir del Menu?", "Mensaje", MB_YESNO | MB_ICONQUESTION);

            if (Opc == IDYES)
            {

                hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwndMenu, Login);
                ShowWindow(hwndMenu, SW_HIDE);
                ShowWindow(hwnd, SW_SHOW);


                /*
                if (hwnd == NULL)
                {
                    hwnd = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hwndMenu, Login);
                    ShowWindow(hwndMenu, SW_HIDE);
                    ShowWindow(hwnd, SW_SHOW);
                }
                */
                hwnd = NULL;
            }
        }break;
        case IDC_BUTTON2:
        {
            if (hwndAlta == NULL)
            {
                hwndAlta = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ALTA), hwndMenu, Alta);
                ShowWindow(hwndMenu, SW_HIDE);
                ShowWindow(hwndAlta, SW_SHOW);
            }
            hwndAlta = NULL;
        }break;


        case IDC_BUTTON3:
        {
            if (hwndModificar == NULL)
            {
                hwndModificar = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MODIFICAR), hwndMenu, Modificar);
                ShowWindow(hwndMenu, SW_HIDE);
                ShowWindow(hwndModificar, SW_SHOW);
            }
            hwndModificar = NULL;
        }break;

        case IDC_BUTTON4:
        {
            if (hwndBaja == NULL)
            {
                hwndBaja = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_BAJA), hwndMenu, Baja);
                ShowWindow(hwndMenu, SW_HIDE);
                ShowWindow(hwndBaja, SW_SHOW);
            }
            hwndBaja = NULL;
        }break;

        case IDC_BUTTON5:
        {
            Citas* actuallist = Inicio;

            if (actuallist != NULL)
            {
                if (hwndAgenda == NULL)
                {
                    hwndAgenda = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_AGENDA), hwndMenu, Agenda);
                    ShowWindow(hwndMenu, SW_HIDE);
                    ShowWindow(hwndAgenda, SW_SHOW);
                }
                hwndAgenda = NULL;
            }
            else
            {
                MessageBox(NULL, "Actualmente no hay listas para mostrar. ¡Agrega nuevas citas para darle vida a la agenda!", "Sin datos disponibles", MB_OK | MB_ICONINFORMATION);
            }

        }break;

        case BTN_CAPTURAR:
        {
            Citas* actuallist = Inicio;

            if (actuallist != NULL)
            {
                int resul = MessageBox(hwndMenu, "Seguro de querer realizar esta accion? al momento de capturar se cerrara por completo el programa", "Saliendo del Programa", MB_YESNO | MB_ICONEXCLAMATION);
                if (resul == IDYES)
                {


                    Capturarveterinarios();

                    MessageBox(hwndMenu, "Ingrese el nombre del archivo para las citas", "AVISO", MB_OK);

                    capturarcitas();
                }
            }
            else
            {
                MessageBox(NULL, "Actualmente no hay listas para mostrar. ¡Agrega nuevas citas!", "Sin datos disponibles", MB_OK | MB_ICONINFORMATION);
            }

        }break;
        }
    }
    break;

    case WM_CLOSE:
    {
        DestroyWindow(hwndMenu);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;
    }
    return 0;
}

LRESULT CALLBACK Alta(HWND hwndAlta, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HWND hButton;

    SYSTEMTIME sysTimeActual;
    GetLocalTime(&sysTimeActual);

    switch (msg)                  /* manipulador del mensaje */
    {
    case WM_INITDIALOG:
    {
        char aux[50];
        GetDlgItemText(hwndAlta, IDC_DATETIMEPICKER1, aux, 50);
        SetDlgItemText(hwndAlta, IDC_StaticFecha, aux);

        bool encontrado = false;
        Veterinario* actual = inicio;
        while (actual != NULL)
        {
            if (strcmp(actual->nombre, nombreUsuario) == 0)
            {
                encontrado = true;
                break;
            }
            actual = actual->sig;
        }
        if (encontrado)
        {
            SetDlgItemText(hwndAlta, IDC_MEDICONOMBRE, actual->nombre);

            // Cargar imagen del doctor

            Veterinario* vetActual = inicio;
            while (vetActual != NULL) {
                if (vetActual == actual) {  // Validar si es el veterinario correcto
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, vetActual->path_foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                    SendDlgItemMessage(hwndAlta, IDC_MEDICOIMAGENALTA, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    break;  // Una vez encontrado el veterinario, sal del bucle
                }
                vetActual = vetActual->sig;
            }
        }

        for (int i = 0; i < 12; i++)
        {
            SendMessage(GetDlgItem(hwndAlta, ID_MASCOTAS), CB_ADDSTRING, 0, (LPARAM)mascota[i]);
        }

        hButton = GetDlgItem(hwndAlta, ID_BTN_CAPTURAR);

        // Deshabilitar el botón al inicio
        EnableWindow(hButton, FALSE);

    }break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case ID_BTN_AGREGARMASCOTA:
        {

            // Obtener el texto del cuadro de edición
            HWND hEditBox = GetDlgItem(hwndAlta, ID_AGREGARMASCOT);
            TCHAR buffer[MAX_TEXT_LENGTH]; // Declarar un búfer para almacenar el texto
            GetWindowText(hEditBox, buffer, MAX_TEXT_LENGTH);

            // Obtener el handle del ComboBox
            HWND hComboBox = GetDlgItem(hwndAlta, ID_MASCOTAS);

            // Agregar el texto al ComboBox
            SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)buffer);

            // Limpiar el cuadro de edición después de agregar el texto
            SetWindowText(hEditBox, ""); // Puedes usar _T("") o L""
        }break;

        case ID_BTN_CAPTURAR: //Boton Capturar
        {
            SYSTEMTIME sysTime;  // Agregar esta línea
            SendMessage(GetDlgItem(hwndAlta, IDC_DATETIMEPICKER1), DTM_GETSYSTEMTIME, 0, (LPARAM)&sysTime);  // Obtener la fecha seleccionada

            intcount = SendDlgItemMessage(hwndAlta, ID_MASCOTAS, CB_GETCOUNT, 0, 0);

            char pivo[50];
            GetDlgItemText(hwndAlta, ID_MASCOTAS, pivo, sizeof(pivo));

            bool encontrado = false;
            //Citas* actual = Inicio;

            for (int i = 0; i < intcount; i++)
            {
                char temp[50];
                SendDlgItemMessage(hwndAlta, ID_MASCOTAS, CB_GETLBTEXT, i, (LPARAM)temp);

                if (strcmp(pivo, temp) == 0)
                {
                    encontrado = true;
                    break;
                }
            }
            if (!encontrado)
            {
                MessageBox(hwndAlta, "Mascota incorrecta. Ingrese una mascota que este en la lista", "Error", MB_OK);
                SetDlgItemText(hwndAlta, ID_MASCOTAS, "");
            }

            else
            {
                if (sysTime.wYear > sysTimeActual.wYear ||
                    (sysTime.wYear == sysTimeActual.wYear && sysTime.wMonth > sysTimeActual.wMonth) ||
                    (sysTime.wYear == sysTimeActual.wYear && sysTime.wMonth == sysTimeActual.wMonth && sysTime.wDay > sysTimeActual.wDay))
                {
                    Citas* nuevacita, * aux = Inicio;

                    nuevacita = new Citas;
                    // Solo convertir en string los numeros.
                    GetDlgItemText(hwndAlta, ID_NAMECLIENTE, nuevacita->NombrCliente, sizeof(nuevacita->NombrCliente));
                    GetDlgItemText(hwndAlta, ID_TELCLIENTE, nuevacita->TelCliente, sizeof(nuevacita->TelCliente));
                    GetDlgItemText(hwndAlta, ID_MASCOTAS, nuevacita->Mascota, sizeof(nuevacita->Mascota));
                    GetDlgItemText(hwndAlta, ID_NAMEMASCOT, nuevacita->NameMascota, sizeof(nuevacita->NameMascota));
                    GetDlgItemText(hwndAlta, ID_MOTIVOCON, nuevacita->MotivoConsulta, sizeof(nuevacita->MotivoConsulta));
                    GetDlgItemText(hwndAlta, ID_COSTO, nuevacita->Costo, sizeof(nuevacita->Costo));

                    nuevacita->fecha = sysTime;

                    if (!Inicio)
                    {
                        // La lista está vacía, el nuevo nodo será el único en la lista
                        Inicio = nuevacita;
                        Inicio->ant = NULL;  // Establecer puntero ant del primer nodo como NULL
                        Inicio->sig = NULL;  // Establecer puntero sig del primer nodo como NULL
                    }
                    else
                    {
                        Citas* actual = Inicio;
                        Citas* anterior = nullptr;

                        //ordenar por fecha
                        /*
                        while (actual && CompareDates(nuevacita->fecha, actual->fecha) < 0)
                        {
                            anterior = actual;
                            actual = actual->sig;
                        }
                        */

                        if (anterior)
                        {
                            anterior->sig = nuevacita;
                            nuevacita->ant = anterior;
                        }
                        else
                        {
                            Inicio = nuevacita;
                        }

                        actual->ant = nuevacita;
                        nuevacita->sig = actual;
                    }

                    MessageBox(hwndAlta, "Archivos Guardados", "Saliendo", MB_OK || MB_ICONQUESTION);
                    if (IDOK)
                    {
                        ShowWindow(hwndAlta, SW_HIDE);
                        ShowWindow(hwndMenu, SW_SHOW);
                        SetDlgItemText(hwndAlta, ID_NAMECLIENTE, "");
                        SetDlgItemText(hwndAlta, ID_TELCLIENTE, "");
                        SetDlgItemText(hwndAlta, ID_MASCOTAS, "");
                        SetDlgItemText(hwndAlta, ID_NAMEMASCOT, "");
                        SetDlgItemText(hwndAlta, ID_MOTIVOCON, "");
                        SetDlgItemText(hwndAlta, ID_COSTO, "");
                    }
                }
                else
                {
                    MessageBox(hwndAlta, "La fecha seleccionada no es valida por favor seleccione una valida", "Advertencia", MB_OK | MB_ICONINFORMATION);

                    SYSTEMTIME sysTimeNew;
                    GetLocalTime(&sysTimeNew);
                    DateTime_SetSystemtime(GetDlgItem(hwndAlta, IDC_DATETIMEPICKER1), GDT_VALID, &sysTimeNew);
                }
            }
        }break;

        case ID_SALIRALTA:
        {
            int resultado = MessageBox(hwndAlta, "¿Seguro que desea salir?", "ADVERTENCIA", MB_YESNO | MB_ICONQUESTION);

            if (resultado == IDYES)
            {
                ShowWindow(hwndAlta, SW_HIDE);
                ShowWindow(hwndMenu, SW_SHOW);
                SetDlgItemText(hwndAlta, ID_NAMECLIENTE, "");
                SetDlgItemText(hwndAlta, ID_TELCLIENTE, "");
                SetDlgItemText(hwndAlta, ID_MASCOTAS, "");
                SetDlgItemText(hwndAlta, ID_NAMEMASCOT, "");
                SetDlgItemText(hwndAlta, ID_MOTIVOCON, "");
                SetDlgItemText(hwndAlta, ID_COSTO, "");
            }
        }break;

        default:
            // Comprueba si el mensaje proviene de un Edit Box
            if (GetDlgItem(hwndAlta, ID_NAMECLIENTE) == (HWND)lParam ||
                GetDlgItem(hwndAlta, ID_TELCLIENTE) == (HWND)lParam ||
                GetDlgItem(hwndAlta, ID_MASCOTAS) == (HWND)lParam ||
                GetDlgItem(hwndAlta, ID_NAMEMASCOT) == (HWND)lParam ||
                GetDlgItem(hwndAlta, ID_MOTIVOCON) == (HWND)lParam ||
                GetDlgItem(hwndAlta, ID_COSTO) == (HWND)lParam)
            {
                // Verifica el contenido de todos los Edit Box
                int textLength1 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_NAMECLIENTE));
                int textLength2 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_TELCLIENTE));
                int textLength3 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_MASCOTAS));
                int textLength4 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_NAMEMASCOT));
                int textLength5 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_MOTIVOCON));
                int textLength6 = GetWindowTextLength(GetDlgItem(hwndAlta, ID_COSTO));

                // Habilita o deshabilita el botón en función del contenido
                EnableWindow(hButton, textLength1 > 0 && textLength2 >= 10 && textLength2 <= 12 && textLength3 > 0 && textLength4 > 0 && textLength5 > 0 && textLength6 > 0);
            }break;
        }
    }break;

    case WM_CLOSE:
    {
        DestroyWindow(hwndAlta);
    }break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }break;
    }

    return 0;
}

LRESULT CALLBACK Baja(HWND hwndBaja, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        bool encontrado = false;
        Veterinario* actual = inicio;
        while (actual != NULL)
        {
            if (strcmp(actual->nombre, nombreUsuario) == 0)
            {
                encontrado = true;
                break;
            }
            actual = actual->sig;
        }
        if (encontrado)
        {
            SetDlgItemText(hwndBaja, IDC_MEDICONOMBRE, actual->nombre);

            // Cargar imagen del doctor
            Veterinario* vetActual = inicio;
            while (vetActual != NULL) {
                if (vetActual == actual) {  // Validar si es el veterinario correcto
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, vetActual->path_foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                    SendDlgItemMessage(hwndBaja, IDC_MEDICOIMAGENBAJA, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    break;  // Una vez encontrado el veterinario, sal del bucle
                }
                vetActual = vetActual->sig;
            }
        }
    }break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_SELECCIONAR_BAJA:
        {

            char pivo[50];
            GetDlgItemText(hwndBaja, IDC_EDITNUM, pivo, sizeof(pivo));

            bool encontrado = false;
            Citas* actual = Inicio;

            while (actual != NULL)
            {
                if (strcmp(actual->NombrCliente, pivo) == 0)
                {
                    encontrado = true;
                    break;
                }
                actual = actual->sig;
            }
            if (encontrado)
            {
                MessageBox(hwnd, "Cuentra encontrada", "Éxito", MB_OK);
            }
            else
            {
                MessageBox(hwnd, "Datos no encontrados.", "Error", MB_OK | MB_ICONERROR);
            }

            char nombreEliminar[50];
            GetDlgItemText(hwndBaja, IDC_EDITNUM, nombreEliminar, 50);
            if (strlen(nombreEliminar) == 0)
            {
                MessageBox(hwndBaja, "No se permite texto vacio", "ADVERTENCIA", MB_OK | MB_ICONEXCLAMATION);
                SetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNUMTELBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITESPECIEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNAMEMASCOTABAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITCOSTOBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITMOTIVOBAJA, "");
            }
            else
            {
                Citas* citaSeleccionada = Inicio;
                while (citaSeleccionada != nullptr)
                {
                    if (strcmp(citaSeleccionada->NombrCliente, nombreEliminar) == 0)
                        //if(citaSeleccionada->NombrCliente == nombreEliminar)
                    {
                        //OutputDebugString(citaSeleccionada->NombrCliente);
                        // Muestra la información de la cita en el control de edición IDC_EDITNAMEBAJA

                        //GetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, citaSeleccionada->NombrCliente, 50);
                        SetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, citaSeleccionada->NombrCliente);
                        SetDlgItemText(hwndBaja, IDC_EDITNUMTELBAJA, citaSeleccionada->TelCliente);
                        SetDlgItemText(hwndBaja, IDC_EDITESPECIEBAJA, citaSeleccionada->Mascota);
                        SetDlgItemText(hwndBaja, IDC_EDITNAMEMASCOTABAJA, citaSeleccionada->NameMascota);
                        SetDlgItemText(hwndBaja, IDC_EDITCOSTOBAJA, citaSeleccionada->Costo);
                        SetDlgItemText(hwndBaja, IDC_EDITMOTIVOBAJA, citaSeleccionada->MotivoConsulta);

                        TCHAR formattedDate[50];

                        wsprintf(formattedDate, TEXT("%d/%02d/%04d"), citaSeleccionada->fecha.wDay, citaSeleccionada->fecha.wMonth, citaSeleccionada->fecha.wYear);
                        SetDlgItemText(hwndBaja, IDC_FECHABAJA, (LPCTSTR)formattedDate);

                        break; // Termina el bucle
                    }
                    citaSeleccionada = citaSeleccionada->sig;
                }
            }
        }break;
        case BTN_SALIR_BAJA:
        {
            int resultado = MessageBox(hwndBaja, "¿Seguro que desea salir?", "ADVERTENCIA", MB_YESNO | MB_ICONQUESTION);

            if (resultado == IDYES)
            {
                ShowWindow(hwndBaja, SW_HIDE);
                ShowWindow(hwndMenu, SW_SHOW);
                SetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNUMTELBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITESPECIEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNAMEMASCOTABAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITCOSTOBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITMOTIVOBAJA, "");
            }
        }break;
        case BTN_ELIMINAR:
        {
            char nombreEliminar[50];
            GetDlgItemText(hwndBaja, IDC_EDITNUM, nombreEliminar, 50);

            if (Inicio == nullptr)
            {
                MessageBox(hwndBaja, "No hay datos almacenados aún.", "Mensaje", MB_OK | MB_ICONINFORMATION);
                SetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNUMTELBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITESPECIEBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITNAMEMASCOTABAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITCOSTOBAJA, "");
                SetDlgItemText(hwndBaja, IDC_EDITMOTIVOBAJA, "");
            }
            else
            {
                Citas* citaAEliminar = Inicio;
                bool encontrado = false;

                while (citaAEliminar != nullptr)
                {
                    if (strcmp(citaAEliminar->NombrCliente, nombreEliminar) == 0) {
                        // Encontraste la cita a eliminar
                        // Aquí puedes eliminarla
                        if (citaAEliminar->ant != nullptr) {
                            citaAEliminar->ant->sig = citaAEliminar->sig;
                        }
                        else
                        {
                            Inicio = citaAEliminar->sig;
                        }
                        if (citaAEliminar->sig != nullptr)
                        {
                            citaAEliminar->sig->ant = citaAEliminar->ant;
                        }
                        delete citaAEliminar; // Eliminar la cita
                        MessageBox(hwndBaja, "Datos eliminados correctamente", "Mensaje", MB_OK | MB_ICONINFORMATION);
                        SetDlgItemText(hwndBaja, IDC_EDITNAMEBAJA, "");
                        SetDlgItemText(hwndBaja, IDC_EDITNUMTELBAJA, "");
                        SetDlgItemText(hwndBaja, IDC_EDITESPECIEBAJA, "");
                        SetDlgItemText(hwndBaja, IDC_EDITNAMEMASCOTABAJA, "");
                        SetDlgItemText(hwndBaja, IDC_EDITCOSTOBAJA, "");
                        SetDlgItemText(hwndBaja, IDC_EDITMOTIVOBAJA, "");
                        encontrado = true;
                        break; // Terminar el bucle
                    }
                    citaAEliminar = citaAEliminar->sig;
                }

                if (!encontrado) {
                    MessageBox(hwndBaja, "Este Nombre o dato no se encuentra en la lista ingrese un nombre o dato existente", "Mensaje", MB_OK | MB_ICONINFORMATION);
                }
            }
        }break;
        }
    }break;

    case WM_CLOSE:
    {
        DestroyWindow(hwndBaja);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;

    }return 0;
}

LRESULT CALLBACK Modificar(HWND hwndModificar, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        bool encontrado = false;
        Veterinario* actual = inicio;
        while (actual != NULL)
        {
            if (strcmp(actual->nombre, nombreUsuario) == 0)
            {
                encontrado = true;
                break;
            }
            actual = actual->sig;
        }
        if (encontrado)
        {
            SetDlgItemText(hwndModificar, IDC_MEDICONOMBRE, actual->nombre);

            // Cargar imagen del doctor
            Veterinario* vetActual = inicio;
            while (vetActual != NULL) {
                if (vetActual == actual) {  // Validar si es el veterinario correcto
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, vetActual->path_foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                    SendDlgItemMessage(hwndModificar, IDC_MEDICOIMAGENMODIFICAR, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    break;  // Una vez encontrado el veterinario, sal del bucle
                }
                vetActual = vetActual->sig;
            }
        }

        for (int i = 0; i < 7; i++)
        {
            SendMessage(GetDlgItem(hwndModificar, CB_MODTIPO), CB_ADDSTRING, 0, (LPARAM)ao[i]);
        }
        HWND hwndFecha = GetDlgItem(hwndModificar, IDC_DATETIMEPICKERMODIFICAR);

        EnableWindow(hwndFecha, FALSE);

    }break;

    case WM_COMMAND:
    {
        Citas* actual = Inicio;

        //validaciones

        char auxil[50], aux[50], auxi[50], pivo[50];

        SYSTEMTIME change;

        SYSTEMTIME tiempoactual;
        GetLocalTime(&tiempoactual);

        HWND hwndFecha = GetDlgItem(hwndModificar, IDC_DATETIMEPICKERMODIFICAR);
        HWND hwndEditbox = GetDlgItem(hwndModificar, IDC_EDITNEWDATO);

        GetDlgItemText(hwndModificar, IDC_NOMBREBUSQUEDAMOD, aux, sizeof(aux));
        GetDlgItemText(hwndModificar, CB_MODTIPO, auxi, sizeof(auxi));
        GetDlgItemText(hwndModificar, IDC_EDITNEWDATO, auxil, sizeof(auxil));

        switch (LOWORD(wParam))
        {
        case BTN_SELECTMOD:
        {

            if (strlen(aux) == 0 || strlen(auxi) == 0)
            {
                MessageBox(hwndModificar, "Rellene todos los campos", "Error", MB_OK | MB_ICONERROR);
            }
            else
            {
                GetDlgItemText(hwndModificar, IDC_NOMBREBUSQUEDAMOD, pivo, sizeof(pivo));

                bool encontrado = false;

                while (actual != NULL)
                {
                    if (strcmp(actual->NombrCliente, pivo) == 0 || strcmp(actual->TelCliente, pivo) == 0 || strcmp(actual->Mascota, pivo) == 0 || strcmp(actual->NameMascota, pivo) == 0 || strcmp(actual->MotivoConsulta, pivo) == 0 || strcmp(actual->Costo, pivo) == 0)
                    {
                        encontrado = true;
                        break;
                    }
                    actual = actual->sig;
                }
                if (encontrado)
                {
                    MessageBox(hwnd, "Cuenta encontrada", "Éxito", MB_OK);
                    indi = -1;
                    indi = SendDlgItemMessage(hwndModificar, CB_MODTIPO, CB_GETCURSEL, 0, 0);

                    if (indi != -1)
                    {
                        SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");

                        switch (indi)
                        {
                        case 0:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->NombrCliente);
                            break;

                        case 1:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->TelCliente);
                            break;

                        case 2:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->NameMascota);
                            break;

                        case 3:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->Mascota);
                            break;

                        case 4:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->MotivoConsulta);
                            break;

                        case 5:

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, actual->Costo);
                            break;

                        case 6:

                            EnableWindow(hwndEditbox, FALSE);
                            EnableWindow(hwndFecha, TRUE);

                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");

                            TCHAR formattedDate[50];
                            wsprintf(formattedDate, TEXT("%d/%02d/%04d"), actual->fecha.wDay, actual->fecha.wMonth, actual->fecha.wYear);
                            SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, (LPCTSTR)formattedDate);
                            break;


                        }
                    }
                    else
                    {
                        int r = MessageBox(hwndModificar, "No se selecciono ningun elemento", "Aviso", MB_OK | MB_ICONERROR);
                    }
                }
                else
                {
                    MessageBox(hwnd, "Datos no encontrados.", "Error", MB_OK | MB_ICONERROR);
                }
            }
        }break;

        case BTN_MODIFICAR:
        {
            SendMessage(hwndFecha, DTM_GETSYSTEMTIME, 0, (LPARAM)&change);

            if (indi == 6)
            {
                if (change.wYear <= tiempoactual.wYear || change.wMonth <= tiempoactual.wMonth || change.wDay <= tiempoactual.wDay)
                {
                    actual->fecha = change;

                    DateTime_SetSystemtime(hwndFecha, GDT_VALID, &tiempoactual);

                    MessageBox(hwndModificar, "Cuenta modificada con éxito", "Aviso", MB_OK | MB_ICONERROR);

                    break;
                }
            }
            else
            {
                if (strlen(aux) == 0 || strlen(auxi) == 0 || strlen(auxil) == 0)
                {
                    MessageBox(hwndModificar, "Rellene todos los campos para proceder", "Aviso", MB_OK | MB_ICONERROR);
                }
                else
                {

                    GetDlgItemText(hwndModificar, IDC_EDITNEWDATO, auxil, sizeof(auxil));

                    switch (indi)
                    {
                    case 0:

                        strcpy_s(actual->NombrCliente, auxil);
                        break;

                    case 1:

                        strcpy_s(actual->TelCliente, auxil);
                        break;

                    case 2:

                        strcpy_s(actual->NameMascota, auxil);
                        break;

                    case 3:

                        strcpy_s(actual->Mascota, auxil);
                        break;

                    case 4:

                        strcpy_s(actual->MotivoConsulta, auxil);
                        break;

                    case 5:

                        strcpy_s(actual->Costo, auxil);
                        break;
                    }

                    MessageBox(hwndModificar, "Cuenta modificada con éxito", "Aviso", MB_OK | MB_RIGHT);

                    SetDlgItemText(hwndModificar, IDC_NOMBREBUSQUEDAMOD, "");
                    SetDlgItemText(hwndModificar, CB_MODTIPO, "");
                    SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                    SetDlgItemText(hwndModificar, IDC_EDITNEWDATO, "");
                    EnableWindow(hwndEditbox, TRUE);
                    ShowWindow(hwndEditbox, SW_HIDE);
                    EnableWindow(hwndFecha, FALSE);
                }
            }


        }break;

        case BTN_SALIRMODIFICAR:
        {
            int resultado = MessageBox(hwndModificar, "¿Seguro que desea salir?", "Advertencia", MB_YESNO | MB_ICONQUESTION);

            if (resultado == IDYES)
            {
                ShowWindow(hwndModificar, SW_HIDE);
                ShowWindow(hwndMenu, SW_SHOW);
                SetDlgItemText(hwndModificar, IDC_NOMBREBUSQUEDAMOD, "");
                SetDlgItemText(hwndModificar, CB_MODTIPO, "");
                SetDlgItemText(hwndModificar, IDC_EDITDATOPREV, "");
                SetDlgItemText(hwndModificar, IDC_EDITNEWDATO, "");
            }
        }break;
        }
    }break;

    case WM_CLOSE:
    {
        DestroyWindow(hwndModificar);
    }
    break;

    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;

    }return 0;
}

LRESULT CALLBACK Agenda(HWND hwndAgenda, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
    {
        bool encontrado = false;
        Veterinario* actual = inicio;
        while (actual != NULL)
        {
            if (strcmp(actual->nombre, nombreUsuario) == 0)
            {
                encontrado = true;
                break;
            }
            actual = actual->sig;
        }
        if (encontrado)
        {
            SetDlgItemText(hwndAgenda, IDC_MEDICONOMBRE, actual->nombre);

            // Cargar imagen del doctor
            Veterinario* vetActual = inicio;
            while (vetActual != NULL)
            {
                if (vetActual == actual)
                {  // Validar si es el veterinario correcto
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, vetActual->path_foto, IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
                    SendDlgItemMessage(hwndAgenda, IDC_MEDICOIMAGENAGENDA, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
                    break;  // Una vez encontrado el veterinario, sal del bucle
                }
                vetActual = vetActual->sig;
            }
        }

        hwndListBox = GetDlgItem(hwndAgenda, IDC_LISTAMOSTRAR);

        Citas* actu = Inicio;
        Citas* indice = nullptr;

        while (actu != nullptr)
        {
            indice = actu->sig;

            while (indice != nullptr)
            {
                // Comparar las fechas completas
                if (actu->fecha.wYear > indice->fecha.wYear ||
                    (actu->fecha.wYear == indice->fecha.wYear &&
                        (actu->fecha.wMonth > indice->fecha.wMonth ||
                            (actu->fecha.wMonth == indice->fecha.wMonth &&
                                actu->fecha.wDay > indice->fecha.wDay))))
                {
                    // Intercambiar solo las fechas
                    SYSTEMTIME tempFecha = actu->fecha;
                    actu->fecha = indice->fecha;
                    indice->fecha = tempFecha;
                }
                indice = indice->sig; // Avanzar al siguiente nodo
            }
            actu = actu->sig; // Avanzar al siguiente nodo para la siguiente iteración del bucle externo
        }


        // Luego de ordenar todas las citas por fecha, agregarlas al listbox
        Citas* citaActual = Inicio;
        while (citaActual != nullptr) {
            TCHAR formattedDate[50];
            wsprintf(formattedDate, TEXT("%d/%02d/%04d"), citaActual->fecha.wDay, citaActual->fecha.wMonth, citaActual->fecha.wYear);

            SendMessage(hwndListBox, LB_ADDSTRING, 0, (LPARAM)formattedDate); // Agregar al listbox

            citaActual = citaActual->sig; // Avanzar al siguiente nodo
        }



    }break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_ELIMINARCITA:
        {
            if (hwndBaja == NULL)
            {
                hwndBaja = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_BAJA), hwndAgenda, Baja);
                ShowWindow(hwndAgenda, SW_HIDE);
                ShowWindow(hwndBaja, SW_SHOW);
            }
            hwndBaja = NULL;
        }break;

        case BTN_MODIFICARCITA:
        {
            if (hwndModificar == NULL)
            {
                hwndModificar = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MODIFICAR), hwndAgenda, Modificar);
                ShowWindow(hwndAgenda, SW_HIDE);
                ShowWindow(hwndModificar, SW_SHOW);
            }
            hwndModificar = NULL;
        }break;

        case BTN_AGREGARCITA:
        {

            if (hwndAlta == NULL)
            {
                hwndAlta = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ALTA), hwndAgenda, Alta);
                ShowWindow(hwndAgenda, SW_HIDE);
                ShowWindow(hwndAlta, SW_SHOW);
            }

            if (hwndAlta != NULL)
            {
                hwndAlta = NULL; // Evitar que se cree otra ventana si ya está abierta

                // Obtener la fecha actual
                SYSTEMTIME nuevaFecha;
                GetLocalTime(&nuevaFecha);

                if (existeCitaEnFecha(nuevaFecha))
                {
                    MessageBox(hwndAgenda, "Ya hay una cita programada para este día.", "Aviso", MB_OK | MB_ICONWARNING);
                    // Aquí puedes mostrar un mensaje o realizar alguna acción adicional
                }
                else
                {
                    // Si no hay una cita para esta fecha, proceder a agregar la nueva cita
                    // Aquí podrías llamar a tu función para agregar una nueva cita
                    // agregarNuevaCita(nuevaFecha, /* otros parámetros de la cita */);
                    // ... (resto de tu lógica para agregar la cita)
                }
            }

        }break;

        case BTN_MOSTRARDATOS:
        {
            Citas* actual = Inicio;
            Citas* indice = nullptr;

            Citas* inicio_copia = Inicio; // Hacer una copia del inicio de la lista

            // Código para buscar la cita seleccionada en el listbox y mostrar sus detalles
            int indice_seleccionado = SendMessage(hwndListBox, LB_GETCURSEL, 0, 0);

            if (indice_seleccionado != LB_ERR)
            {
                TCHAR fechaSeleccionada[50];
                SendMessage(hwndListBox, LB_GETTEXT, indice_seleccionado, (LPARAM)fechaSeleccionada);

                // Recorrer la lista para encontrar la cita con la fecha seleccionada
                Citas* citaSeleccionada = Inicio;

                while (citaSeleccionada != nullptr)
                {
                    TCHAR formattedDate[50];
                    wsprintf(formattedDate, TEXT("%d/%02d/%04d"), citaSeleccionada->fecha.wDay, citaSeleccionada->fecha.wMonth, citaSeleccionada->fecha.wYear);

                    if (strcmp(fechaSeleccionada, formattedDate) == 0)
                    {
                        // Mostrar detalles de la cita seleccionada
                        SetDlgItemText(hwndAgenda, IDC_NOMBREMOD, citaSeleccionada->NombrCliente);
                        SetDlgItemText(hwndAgenda, IDC_TELMOD, citaSeleccionada->TelCliente);
                        SetDlgItemText(hwndAgenda, IDC_NOMBREMASCOTAMOD, citaSeleccionada->NameMascota);
                        SetDlgItemText(hwndAgenda, IDC_ESPECIEMOD, citaSeleccionada->Mascota);
                        SetDlgItemText(hwndAgenda, IDC_MOTIVOMOD, citaSeleccionada->MotivoConsulta);
                        SetDlgItemText(hwndAgenda, IDC_COSTOMOD, citaSeleccionada->Costo);

                        SetDlgItemText(hwndAgenda, IDC_FECHAMOD, fechaSeleccionada);

                        break; // Salir del bucle una vez que se haya encontrado la cita
                    }

                    citaSeleccionada = citaSeleccionada->sig; // Avanzar al siguiente nodo
                }
            }

        }break;

        case BTN_SALIRAGENDA:
        {
            int resultado = MessageBox(hwndAgenda, "¿Seguro que desea salir?", "Advertencia", MB_YESNO | MB_ICONQUESTION);

            if (resultado == IDYES)
            {
                ShowWindow(hwndAgenda, SW_HIDE);
                ShowWindow(hwndMenu, SW_SHOW);
                SetDlgItemText(hwndAgenda, IDC_NOMBREMOD, "");
                SetDlgItemText(hwndAgenda, IDC_TELMOD, "");
                SetDlgItemText(hwndAgenda, IDC_NOMBREMASCOTAMOD, "");
                SetDlgItemText(hwndAgenda, IDC_MOTIVOMOD, "");
                SetDlgItemText(hwndAgenda, IDC_COSTOMOD, "");
                SetDlgItemText(hwndAgenda, IDC_FECHAMOD, "");
            }
        }break;
        }
    }
    }return 0;
}

void AgregarVeterinario(char nombre[], char cedula[], char usuario[], char contraseña[], char path_foto[])
{
    Veterinario* nuevoVeterinario = new Veterinario;
    strcpy_s(nuevoVeterinario->nombre, 100, nombre);
    strcpy_s(nuevoVeterinario->cedula, 50, cedula);
    strcpy_s(nuevoVeterinario->clave, 50, usuario);
    strcpy_s(nuevoVeterinario->contraseña, 50, contraseña);
    strcpy_s(nuevoVeterinario->path_foto, 260, path_foto);
    nuevoVeterinario->sig = inicio;
    nuevoVeterinario->ant = NULL;

    if (inicio != NULL)
    {
        inicio->ant = nuevoVeterinario;
    }

    inicio = nuevoVeterinario;
}

void Capturarveterinarios()
{
    OPENFILENAME sfn;
    char FilePath[300] = { 0 };
    char FileName[300] = { 0 };

    ZeroMemory(&sfn, sizeof(sfn));

    sfn.lStructSize = sizeof(sfn);
    sfn.hwndOwner = hwnd;
    sfn.lpstrFilter = "BINARIO\0  * .bin*\0Archivo de texto\0 * .txt\0";
    sfn.lpstrFile = FilePath;
    sfn.nMaxFile = sizeof(FilePath);
    sfn.lpstrFileTitle = FileName;
    sfn.nMaxFileTitle = sizeof(FileName);
    sfn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    sfn.nFilterIndex = 2;
    sfn.lpstrDefExt = "txt";


    if (GetSaveFileName(&sfn))
    {
        ofstream var(FilePath); // Abre el archivo seleccionado para escritura

        if (!var.is_open())
        {
            MessageBox(NULL, "No se pudo abrir el archivo para escritura.", "Error", MB_OK | MB_ICONERROR);
            return;
        }

        Veterinario* aux = inicio; // Suponiendo que 'inicio' es el puntero al inicio de la lista de veterinarios

        while (aux != nullptr)
        {
            var << aux->nombre << endl;
            var << aux->cedula << endl;
            var << aux->clave << endl;
            var << aux->contraseña << endl;
            var << aux->path_foto << endl;
            //var << endl;

            aux = aux->sig; // Avanza al siguiente nodo de la lista
        }

        var.close(); // Cierra el archivo
        MessageBox(NULL, "Datos guardados en el archivo con éxito.", "Éxito", MB_OK);
    }
    else
    {
        MessageBox(NULL, "No se seleccionó ningún archivo", "Aviso", MB_OK | MB_ICONWARNING);
    }
}

void capturarcitas()
{
    OPENFILENAME sfn;
    char FilePath[300] = { 0 };
    char FileName[300] = { 0 };

    ZeroMemory(&sfn, sizeof(sfn));

    sfn.lStructSize = sizeof(sfn);
    sfn.hwndOwner = hwndMenu; // Cambia esto si tienes una ventana principal
    sfn.lpstrFilter = "BINARIO\0  * .bin*\0Archivo de texto\0 * .txt\0";
    sfn.lpstrFile = FilePath;
    sfn.nMaxFile = sizeof(FilePath);
    sfn.lpstrFileTitle = FileName;
    sfn.nMaxFileTitle = sizeof(FileName);
    sfn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
    sfn.nFilterIndex = 2;
    sfn.lpstrDefExt = "txt";

    if (GetSaveFileName(&sfn))
    {
        ofstream var(FilePath); // Abre el archivo seleccionado para escritura

        if (!var.is_open())
        {
            MessageBox(NULL, "No se pudo abrir el archivo para escritura.", "Error", MB_OK | MB_ICONERROR);
            return;
        }

        Citas* aux = Inicio; // Suponiendo que 'inicio' es el puntero al inicio de la lista de veterinarios

        while (aux != nullptr)
        {
            var << aux->NombrCliente << endl;
            var << aux->TelCliente << endl;
            var << aux->Mascota << endl;
            var << aux->NameMascota << endl;
            var << aux->MotivoConsulta << endl;
            var << aux->Costo << endl;
            var << aux->fecha.wDay << "/" << aux->fecha.wMonth << aux->fecha.wYear << endl;
            aux = aux->sig; // Avanza al siguiente nodo de la lista
        }

        var.close(); // Cierra el archivo
        MessageBox(NULL, "Datos guardados en el archivo con éxito.", "Éxito", MB_OK);
    }
    else
    {
        MessageBox(NULL, "No se seleccionó ningún archivo", "Aviso", MB_OK | MB_ICONWARNING);
    }
    DestroyWindow(hwnd);
}

void Datos_Cargar()
{
    const char* fileName = "C:\\Users\\Ferdi\\OneDrive\\Escritorio\\Avance Uno PA\\Citas\\Citas.bin"; // Cambia la ruta para sisterma de computo

    ifstream inFile(fileName, ios::binary); // Abrir en modo binario

    if (!inFile.is_open())
    {
        MessageBox(hwndLogin, "No se pudo abrir el archivo binario de citas", "Aviso", MB_OK | MB_ICONERROR);
        return;
    }

    Citas* nuevoCliente = nullptr;
    Citas* ultimoCliente = nullptr;

    while (true)
    {
        nuevoCliente = new Citas(); // Asignar memoria para cada nueva cita

        // Leer datos desde el archivo binario hacia el nuevo objeto Citas
        if (!inFile.read(reinterpret_cast<char*>(nuevoCliente), sizeof(Citas)))
        {
            // Si la lectura falla o llegamos al final del archivo, liberar la memoria y salir del bucle
            delete nuevoCliente;
            break;
        }

        nuevoCliente->sig = nullptr;

        if (Inicio == nullptr)
        {
            // La lista está vacía, establecer la nueva cita como inicio
            Inicio = nuevoCliente;
        }
        else
        {
            // La lista no está vacía, establecer la nueva cita como siguiente de la última cita
            ultimoCliente->sig = nuevoCliente;
            nuevoCliente->ant = ultimoCliente;
        }

        // Actualizar el puntero a la última cita
        ultimoCliente = nuevoCliente;
    }

    inFile.close();
}

void Datos_Guardar()
{
    const char* fileName = "C:\\Users\\Ferdi\\OneDrive\\Escritorio\\Avance Uno PA\\Citas\\Citas.bin"; // Cambia la ruta para sisterma de computo

    ofstream outFile(fileName, ios::binary); // Abrir en modo binario

    if (!outFile.is_open())
    {
        MessageBox(hwndLogin, "No se pudo crear el archivo binario de citas", "Aviso", MB_OK | MB_ICONERROR);
        return;
    }

    Citas* actual = Inicio; // Suponiendo que 'Inicio' es el puntero al inicio de la lista de citas

    while (actual != nullptr)
    {
        // Escribir los datos de cada cita en el archivo binario
        outFile.write(reinterpret_cast<char*>(actual), sizeof(Citas));
        actual = actual->sig;
    }

    outFile.close();
}

void Datos_Veterinario_Cargar()
{
    const char* fileName = "C:\\Users\\Ferdi\\OneDrive\\Escritorio\\Avance Uno PA\\Medicos\\Medicos.bin"; // Ruta al archivo binario para Veterinarios (Cambiar para cada sistema)

    ifstream inFile(fileName, ios::binary); // Abrir en modo binario

    if (!inFile.is_open())
    {
        MessageBox(hwndLogin, "No se pudo abrir el archivo binario de veterinarios", "Aviso", MB_OK | MB_ICONERROR);
        return;
    }

    Veterinario* nuevoVeterinario = nullptr;
    Veterinario* ultimoVeterinario = nullptr;

    while (true)
    {
        nuevoVeterinario = new Veterinario(); // Asignar memoria para cada nuevo Veterinario

        // Leer datos desde el archivo binario hacia el nuevo objeto Veterinario
        if (!inFile.read(reinterpret_cast<char*>(nuevoVeterinario), sizeof(Veterinario)))
        {
            // Si la lectura falla o llegamos al final del archivo, liberar la memoria y salir del bucle
            delete nuevoVeterinario;
            break;
        }

        nuevoVeterinario->sig = nullptr;

        if (inicio == nullptr)
        {
            // La lista está vacía, establecer el nuevo veterinario como inicio
            inicio = nuevoVeterinario;
        }
        else
        {
            // La lista no está vacía, establecer el nuevo veterinario como siguiente del último veterinario
            ultimoVeterinario->sig = nuevoVeterinario;
            nuevoVeterinario->ant = ultimoVeterinario;
        }

        // Actualizar el puntero al último veterinario
        ultimoVeterinario = nuevoVeterinario;
    }

    inFile.close();
}

void Datos_Veterinario_Guardar()
{
    const char* fileName = "C:\\Users\\Ferdi\\OneDrive\\Escritorio\\Avance Uno PA\\Medicos\\Medicos.bin"; // Ruta al archivo binario para Veterinarios (Cambiar para cada sistema)

    ofstream outFile(fileName, ios::binary); // Abrir en modo binario

    if (!outFile.is_open())
    {
        MessageBox(hwndLogin, "No se pudo crear el archivo binario de veterinarios", "Aviso", MB_OK | MB_ICONERROR);
        return;
    }

    Veterinario* actual = inicio; // Suponiendo que 'inicioVeterinarios' es el puntero al inicio de la lista de veterinarios

    while (actual != nullptr)
    {
        // Escribir los datos de cada veterinario en el archivo binario
        outFile.write(reinterpret_cast<char*>(actual), sizeof(Veterinario));
        actual = actual->sig;
    }

    outFile.close();
}

bool existeCitaEnFecha(SYSTEMTIME nuevaFecha) {
    Citas* citaActual = Inicio;

    while (citaActual != nullptr) {
        if (citaActual->fecha.wYear == nuevaFecha.wYear &&
            citaActual->fecha.wMonth == nuevaFecha.wMonth &&
            citaActual->fecha.wDay == nuevaFecha.wDay) {
            return true; // Ya hay una cita para esta fecha
        }
        citaActual = citaActual->sig;
    }

    return false; // No hay citas para esta fecha
}

