# -*- coding: utf-8 -*-
"""
Spyder Editor
"""

# !/usr/bin/python3

import serial

import tkinter as tk
from tkinter import *
from tkinter import ttk
import threading
import queue
import time
from tkinter import messagebox

root = tk.Tk()
DEBUG_trama_check = IntVar()
DEBUG_Moduls_check = IntVar()
DEBUG_Consola_check = IntVar()
DEBUG_trama_check.set(1)
DEBUG_Moduls_check.set(0)
DEBUG_Consola_check.set(0)
DEBUG_trama = DEBUG_trama_check.get()
DEBUG_Moduls = DEBUG_Moduls_check.get()
DEBUG_Consola = DEBUG_Consola_check.get()

port_com  = 'COM4'
baud_rate = 115200

INSTR_IDLE = 0x00
INSTR_END = 0xFF
INSTR_STOP_THREAD = 0xEE

INSTR_PING = 0x01
INSTR_READ = 0x02
INSTR_WRITE = 0x03
INSTR_REG_WR = 0x04
INSTR_ACTION = 0x05
INSTR_RESET = 0x06
INSTR_SYNC_WRT = 0x83

ID_L = 0x01
ID_R = 0x02
CCW = 0x00
CW = 0x04

n_motors = 2
ID = 0
trama = (b'')

instruccio = INSTR_IDLE

instruction_set = {
        0x00: "IDLE",
        0x01: "PING",
        0x02: "READ",
        0x03: "WRITE",
        0x04: "REG_WRITE",
        0x05: "ACTION",
        0x06: "RESET",
        0x83: "SYNC_WRITE",
        0xFF: "END"
}

AX12_memory = {
        0x00: "Model Number(L)",
        0x01: "Model Number(H)",
        0x02: "Firmware Version",
        0x03:  "ID",
        0x04: "Baud Rate",
        0x05: "Return Delay Time",
        0x06: "CW Angle Limit(L)",
        0x07: "CW Angle Limit(H)",
        0x08: "CCW Angle Limit(L)",
        0x09: "CCW Angle Limit(H)",
        0x0A: "Reserved",
        0x0B: "High Temp. Limit",
        0x0C: "Low Voltage Limit",
        0x0D: "High Voltage Limit",
        0x0E: "Max Torque(L)",
        0x0F: "Max Torque(H)",
        0x10: "Status Return Level",
        0x11: "Alarm LED",
        0x12: "Alarm Shoutdown",
        0x13: "Reserved",
        0x14: "Down Calibration(L)",
        0x15: "Down Calibration(H)",
        0x16: "Up Calibration(L)",
        0x17: "Up Calibration(H)",
        0x18: "Torque Enable",
        0x19: "LED",
        0x1A: "CW Compliance Margin",
        0x1B: "CCW COmpliance Margin",
        0x1C: "CW Compliance Slope",
        0x1D: "CCW Compliance Slope",
        0x1E: "Goal Position(L)",
        0x1F: "Goal Position(H)",
        0x20: "Moving Speed(L)",
        0x21: "Moving Speed(H)",
        0x22: "Torque Limit(L)",
        0x23: "Torque Limit(H)",
        0x24: "Present Position(L)",
        0x25: "Present Position(H)",
        0x26: "Present Speed(L)",
        0x27: "Present Speed(H)",
        0x28: "Present Load(L)",
        0x29: "Present Load(H)",
        0x2A: "Present Voltage",
        0x2B: "Present Temperature",
        0x2C: "Registered Instruction",
        0x2D: "ADC_value",
        0x2E: "Moving",
        0x2F: "Lock",
        0x30: "Punch(L)",
        0x31: "Punch(H)"
        }


def f_led():
    global ID
    if DEBUG_Consola == 1:
        if AX12[ID-1][0x19] == 1:
            print("LED motor", ID, "ON")
        else:
            print("LED motor", ID, "OFF")
    return


def f_moving_speed():
    global ID
    global AX12_moving_L
    global AX12_moving_R
    velocitat = AX12[ID-1][0x20] + (AX12[ID-1][0x21] & 0x03)*256
    if DEBUG_Consola == 1:
        print("Velocitat  motor", ID, "=", velocitat)
    text_motor = "v="+str(velocitat)
    sentit = AX12[ID-1][0x21] & 0x04
    if sentit == 0:
        if DEBUG_Consola == 1:
            print("Sentit gir motor", ID, "= CCW")
        if ID ==1:
            AX12_moving_L = text_motor + " CCW"
        else:
            AX12_moving_R = text_motor + " CCW"
    else:
        if DEBUG_Consola == 1:
            print("Sentit gir motor", ID, "= CW")
        if ID ==1:
            AX12_moving_L = text_motor + " CW"
        else:
            AX12_moving_R = text_motor + " CW"
    return


def f_angle_limit():
    global ID
    cw_angle = AX12[ID-1][0x06] + (AX12[ID-1][0x07] & 0x03)*256
    ccw_angle = AX12[ID-1][0x08] + (AX12[ID-1][0x09] & 0x03)*256
    if DEBUG_Consola == 1:
        if cw_angle == 0:
            print("Motor", ID, "gir continu en sentit horari")
        else:
            print("Motor", ID, "angle limit en sentit horari:", cw_angle)
        if ccw_angle == 0:
            print("Motor", ID, "gir continu en sentit anti-horari")
        else:
            print("Motor", ID, "angle limit en sentit anti-horari:", ccw_angle)
    return    


def f_ADC_value():
    global ID
    ADC_val = AX12[ID-1][0x2D]
    if DEBUG_Consola == 1:
        print("Mesura del ADC", ID, ":", ADC_val)
    return
    


def AX12_func(argument):
    switcher= {       
#           0x00: "Model Number(L)",
#           0x01: "Model NUmber(H)",
#           0x02: "Firmware Version",
#           0x03:  "ID",
#           0x04: "Baud Rate",
#           0x05: "Return Delay Time",
            0x06: f_angle_limit,
            0x07: f_angle_limit,
            0x08: f_angle_limit,
            0x09: f_angle_limit,
#           0x0A: "Reserved",
#           0x0B: "High Temp. Limit",
#           0x0C: "Low Voltage Limit",
#           0x0D: "High Voltage Limit",
#           0x0E: "Max Torque(L)",
#           0x0F: "Max Torque(H)",
#           0x10: "Status Return Level",
#           0x11: "Alarm LED",
#           0x12: "Alarm Shoutdown",
#           0x13: "Reserved",
#           0x14: "Down Calibration(L)",
#           0x15: "Down Calibration(H)",
#           0x16: "Up Calibration(L)",
#           0x17: "Up Calibration(H)",
#           0x18: "Torque Enable",
            0x19: f_led,
#           0x1A: "CW Compliance Margin",
#           0x1B: "CCW COmpliance Margin",
#           0x1C: "CW Compliance Slope",
#           0x1D: "CCW Compliance Slope",
#           0x1E: "Goal Position(L)",
#           0x1F: "Goal Position(H)",
            0x20: f_moving_speed,
            0x21: f_moving_speed,
#           0x22: "Torque Limit(L)",
#           0x23: "Torque Limit(H)",
#           0x24: "Present Position(L)",
#           0x25: "Present Position(H)",
#           0x26: "Present Speed(L)",
#           0x27: "Present Speed(H)",
#           0x28: "Present Load(L)",
#           0x29: "Present Load(H)",
#           0x2A: "Present Voltage",
#           0x2B: "Present Temperature",
#           0x2C: "Registered Instruction",
            0x2D: f_ADC_value,
#           0x2E: "Moving",
#           0x2F: "Lock",
#           0x30: "Punch(L)",
#           0x31: "Punch(H)"
            }
    func = switcher.get(argument, lambda: "Funció no implementada")
    func()

def reset_modul_AX12(id_modul):
    AX12[id_modul-1][0x00] = 0x0C       #Model Number(L)
    AX12[id_modul-1][0x02] = 0x01       #Model Number(H)
    AX12[id_modul-1][0x03] = id_modul   #ID
    AX12[id_modul-1][0x04] = 0x01       #Baud Rate
    AX12[id_modul-1][0x05] = 0xFA       #Return Delay Time
    AX12[id_modul-1][0x06] = 0x00       #CW Angle Limit(L) 
    AX12[id_modul-1][0x07] = 0x00       #CW Angle Limit(H)
    AX12[id_modul-1][0x08] = 0xFF       #CCW Angle Limit(L)
    AX12[id_modul-1][0x09] = 0x03       #CW Angle Limit(H)
    AX12[id_modul-1][0x0B] = 0x55       #Highest Limit Temperature
    AX12[id_modul-1][0x0C] = 0x3C       #Lowest Limit Voltage
    AX12[id_modul-1][0x0D] = 0xBE       #Highest Limit Voltage
    AX12[id_modul-1][0x0E] = 0xFF       #Max Torque(L)
    AX12[id_modul-1][0x0F] = 0x03       #Max Torque(H)
    AX12[id_modul-1][0x10] = 0x02       #Status Return Level
    AX12[id_modul-1][0x11] = 0x04       #Alarm LED
    AX12[id_modul-1][0x12] = 0x04       #Alarm Shoutdown
    AX12[id_modul-1][0x22] = AX12[id_modul-1][0x0E]   #Torque Limit(L)
    AX12[id_modul-1][0x23] = AX12[id_modul-1][0x0F]   #Torque Limit(H)
    return


# comprova si hi ha error de checksum a la trama
def comprova_checksum(frame):
    len_trama = len(frame)
    chk_sum = 0
    for index in range(2, (len_trama-1)):
        chk_sum = chk_sum+frame[index]
    chk_sum = chk_sum & 0xFF
    if (chk_sum | frame[len_trama-1]) == 0xFF:
        #       ser.write(b'\x00')
        if DEBUG_trama:
            print('Checksum correcte')
        return 0x00
    else:
        print('Error de Checksum')
#        ser.write(b'\x10')
        return 0x10

# comprova si hi ha error d'instruccio la trama
def comprova_instr(instruccio):
    if (instruccio < 0x07) or (instruccio == 0xFF) or (instruccio == 0x83):
        if DEBUG_Consola == 1:
            print("Instrucció:", instruction_set[instruccio])
        return 0x00
    else:
        print("Error d'instruccio")
        return 0x70


def send_status_packet(modul_id, instruccio, error_code):
    status_frame = Status_NoError
    status_frame[2] = modul_id
    status_frame[4] = error_code
    len_trama = len(status_frame)
    l_chksum = 0
    for index in range(2, (len_trama-2)):
        l_chksum = l_chksum+status_frame[index]
    l_chksum = (~l_chksum & 0xFF)
    status_frame[len_trama-1] = l_chksum
    string = ''.join(['0x%02X ' %b for b in status_frame])
    if DEBUG_trama:
        print("status packet in hex:", string)
        print("status packet in dec:", status_frame)
    ser.write(status_frame)
    return


def generate_read_packet():
    return


def generate_status_packed(id_modul, instruc, code_error):
    if instruc != 2:
        send_status_packet(id_modul, instruc, code_error)
    else:
        generate_read_packet()
    return
    

#dona l'estat del robot 
def robot_status():
    global ESTAT_ROBOT
    if DEBUG_Consola == 1:
        print ("----------- ESTAT DEL ROBOT -----------------------")
    v_left = AX12[ID_L-1][0x20] + (AX12[ID_L-1][0x21] & 0x03)*256
    sentit_left = AX12[ID_L-1][0x21] & 0x04
    v_right = AX12[ID_R-1][0x20] + (AX12[ID_R-1][0x21] & 0x03)*256
    sentit_right = AX12[ID_R-1][0x21] & 0x04
    if (v_left == 0) & (v_right == 0):
        if DEBUG_Consola == 1:
            print("Robot Parat")
        ESTAT_ROBOT="Robot Parat"
    elif sentit_left == sentit_right: #si motors giren mateix sentit => robot gira
        if sentit_left == CW:
            if DEBUG_Consola == 1:
                print("Robot Gira Esquerra")
            ESTAT_ROBOT="Robot Gira Esquerra"
        else:
            if DEBUG_Consola == 1:
                print("Robot Gira Dreta")
            ESTAT_ROBOT="Robot Gira Dreta"
    elif abs(v_left-v_right)<20: #si motors giren sentit contrari a mateixa velocitat=> robot va recte
        if sentit_left == CW:
            if DEBUG_Consola == 1:
                print("Robot Marxa Enrere")
            ESTAT_ROBOT="Robot Marxa Enrere"
        else:
            if DEBUG_Consola == 1:
                print("Robot Marxa Endavant")
            ESTAT_ROBOT="Robot Marxa Endavant"
    elif v_left > v_right: #velocitats diferents, motor esquerre mes rapid
        if sentit_left == CW:
            if DEBUG_Consola == 1:
                print("Robot Gira Esquerra")
            ESTAT_ROBOT="Robot Gira Esquerra"
        else:
            if DEBUG_Consola == 1:
                print("Robot Gira Dreta")
            ESTAT_ROBOT="Robot Gira Dreta"
    elif sentit_left == CW: #velocitats diferents, motor dret mes rapid
        if DEBUG_Consola == 1:
            print("Robot Gira Dreta")
        ESTAT_ROBOT="Robot Gira Dreta"
    else:
        if DEBUG_Consola == 1:
            print("Robot Gira Esquerra")
        ESTAT_ROBOT="Robot Gira Esquerra"
    return
     

def print_AX_MemoryMap():
    print("---------------------------------------------------")    
    print("========== MOTOR ======== [1] ======= [2]")
    for i in range(50):
        mot1 = ''.join(['0x%02X ' %(AX12[0][i])])   #format Hexadecimal
        mot2 = ''.join(['0x%02X ' %(AX12[1][i])])   #format Hexadecimal
        print('{:-<23}'.format(AX12_memory[i]), ">", mot1, "     ", mot2)
    print("####################################################")
    return


def Actualitza_AX_Memory(id_modul, adressa, nparametres):
    global trama
    for index in range(nparametres):
        AX12[id_modul-1][adressa+index] = trama[index+6]
    return


def print_trama():
    print(" ")
    print("####################################################")
    print("Received Instruction Frame:", trama)
    for index in range(len(trama)):
        print("Received Instruction Frame[", index, "]:", '0x%02X ' %trama[index])
    print("####################################################")
    return


def print_tipus_Instruccio(instruccio, comandament):
    error_de_instr = comprova_instr(instruccio)
    if DEBUG_Consola == 1:
        if error_de_instr == 0:
            print("----------- INSTRUCCIÓ i COMANDAMENT --------------")   
            print("Command:", AX12_memory[comandament])
    return error_de_instr


# iNICIALITZACIÓ
#inicialitza els moduls a valors de reset
AX12 = [[0] * 50 for i in range(n_motors)] 
reset_modul_AX12(1)
reset_modul_AX12(2)
#crea una trama d'status per indicar que no hi ha error
Status_NoError = [0xff, 0xff, 0x01, 0x02, 0x00, 0xfc]
#inicialitza el port serie
try:
    ser = serial.Serial(port_com, baud_rate, timeout=0)
except:
    mensaje = "No se puede abrir el puerto\n\t" + port_com
    messagebox.showerror("Error COM", mensaje)
    quit()

#aquesta instruccio es no fer res
instruccio = INSTR_IDLE
#si esta activat el debug dels moduls presenta el contingut de memoria d'aquests
if DEBUG_Moduls:
    print_AX_MemoryMap()

## Funciones para gestionar la aplicacion grafica
    
class Hilo(threading.Thread):
    def __init__(self, nombre, cola, cola_hilo, barra_izq, barra_der, label_trama, label_motor_left, label_motor_right):
        threading.Thread.__init__(self)
        self.cola = cola
        self.cola_hilo = cola_hilo
        self.barra_izq = barra_izq
        self.barra_der = barra_der
        self.label_trama = label_trama
        self.label_motor_left = label_motor_left
        self.label_motor_right = label_motor_right

    def leer_puerto(self, la_cola, la_cola_hilo, barra_izq, barra_der, label_trama, label_motor_left, label_motor_right):
        global instruccio
        global trama
        global ID
       # global ESTAT_ROBOT
        global AX12_moving_L
        global AX12_moving_R
        AX12_moving_L = "PARAT"
        AX12_moving_R = "PARAT"
        if DEBUG_Consola == 1:
            print("Lectura puerto iniciada...", instruccio)
        while (instruccio != INSTR_END):
            if not la_cola.empty():
                mensaje = la_cola.get()
                if (mensaje == INSTR_END):
                    instruccio = mensaje
                    if DEBUG_Consola == 1:
                        print("instruccio rebuda: ", instruccio)
                if DEBUG_Consola == 1:
                    print("mensaje recibido en leer_puerto: ", mensaje)                
            else:
#                robot_status()
#                print("****************************************************")
                #espera a que arribin dades pel port serie
#                trama = ser.read(100)
#                while trama == (b''):
    
                trama = ser.read(16)
                if trama != (b''):
                    if DEBUG_Consola == 1:
                        print ("")
                        print("****************************************************")
                    instruccio = trama[4] #posicio de la trama on esta la instruccio
                    #si esta activat el debug mostra la tama que arriba
                    if DEBUG_trama:
                        print_trama()
                    #mira quina instruccio es i si no es una de les que existeix dona un error    
                    instr_error = print_tipus_Instruccio(instruccio, trama[5])
                    #copmprova el checksum rebut amb el calculat
                    chk_sum_error = comprova_checksum(trama)
                    #error indicara si hi ha un error, sigui d'instruccio o de checksum
                    error = (chk_sum_error | instr_error)
                    ID = trama[2] #posicio a la trama del identificador edl modul
                    if ID != 0xFE: #si el ID no es el de broadcast respon amb un status packet
                        send_status_packet(ID, instruccio, error)
                    else:
                        print("Broadcasting ID Instruction Packet")
                    if (error == 0): #si no hi ha hagut cap error analitza la instruccio i l'executa
                        if (instruccio == INSTR_WRITE): #per ara nomes executa la instruccio WRITE
                            n_parametres = trama[3]-3
                            address = trama[5] #posicio de la tama on esta l'adreá de memoria del modula escriure
                            Actualitza_AX_Memory(ID, address, n_parametres)
                            AX12_func(address) #informa quin comandament s'ha executat
                            if DEBUG_Moduls:
                                print_AX_MemoryMap()
                            robot_status()  
                            #label_trama.set(str(trama)[3:-1])
                            label_trama.set(ESTAT_ROBOT)
                            label_motor_left.set(AX12_moving_L)
                            label_motor_right.set(AX12_moving_R)
                            

                # realizar una tarea
                time.sleep(0.01)
#                valor1 = barra_izq["value"]
#                valor1 = valor1 + 1
#                valor1 = 
#                if(valor1 > barra_izq["maximum"]):
#                    valor1 = 0
                barra_izq["value"] = AX12[0][0x2D]
        
                # echar otra tarea
#                valor2 = barra_der["value"]
#                valor2 = valor2 -1
#                if(valor2 < 0):
#                    valor2 = barra_der["maximum"]
                barra_der["value"] = AX12[1][0x2D]

                
                
        la_cola_hilo.put(INSTR_STOP_THREAD)
        if DEBUG_Consola == 1:
            print("lectura parada")

    def run(self):
        self.leer_puerto(self.cola, self.cola_hilo, self.barra_izq, self.barra_der, 
                         self.label_trama, self.label_motor_left, self.label_motor_right)
        if DEBUG_Consola == 1:
            print("funcion run terminada")
         
class Application(tk.Frame):
    def __init__(self, master=None):
        super().__init__(master)
        self.master = master
        self.texto_trama = StringVar()
        self.texto_motor_left = StringVar()
        self.texto_motor_right = StringVar()
##        self.pack()
        self.grid()
        self.create_widgets()
        self.crear_hilo()
        self.check_queue()

    def create_widgets(self):
        self.spacer_up = Label(self, width= 5, height = 1)
        self.spacer_up.grid(row = 0, column = 0)
        self.spacer_bottom = Label(self, width= 5, height = 1)
        self.spacer_bottom.grid(row = 8, column = 3)
        
        self.hi_there = tk.Button(self)
        self.hi_there["text"] = "About"
        self.hi_there["fg"] = "blue"
        self.hi_there["command"] = self.say_hi
        self.hi_there.grid(row=7, column=3)
        
        self.Debug_frame = Checkbutton(self, variable=DEBUG_trama_check)
        self.Debug_frame["text"] = "Debug Trames"
        self.Debug_frame["fg"] = "blue"
        self.Debug_frame["command"] = self.set_debug_frames
        self.Debug_frame.grid(row=1, column=1)
        
        self.Debug_module = Checkbutton(self, variable=DEBUG_Moduls_check)
        self.Debug_module["text"] = "Debug Moduls"
        self.Debug_module["fg"] = "blue"
        self.Debug_module["command"] = self.set_debug_moduls
        self.Debug_module.grid(row=1, column=2)
        
        self.Debug_consola = Checkbutton(self, variable=DEBUG_Consola_check)
        self.Debug_consola["text"] = "Consola On/off"
        self.Debug_consola["fg"] = "blue"
        self.Debug_consola["command"] = self.set_debug_consola
        self.Debug_consola.grid(row=1, column=3)
     
        self.label_AX12_L = Label(self, text="MOTOR Esq.:")
        self.label_AX12_L.grid(row=2, column=1)
        self.label_motor_left = Label(self, textvariable = self.texto_motor_left)
        self.texto_motor_left.set("PARAT")
        self.label_motor_left.grid(row=2, column=2)     
    
        self.label_izq = Label(self, text="LDR Esq.")
        self.label_izq.grid(row=3, column=1)
        
        self.progress_bar_izq = ttk.Progressbar(self, orient="horizontal",
                                        length=255, maximum=255,
                                        mode="determinate",)
        self.progress_bar_izq["value"] = 0
        self.progress_bar_izq.place(relx=0.5, rely=0.5, relwidth=0.80,  anchor=tk.CENTER)
        self.progress_bar_izq.grid(row=3, column=2)
   
        self.label_robot = Label(self, text="ESTAT ROBOT:")
        self.label_robot.grid(row=4, column=1)
        self.label_trama = Label(self, textvariable = self.texto_trama)
        self.texto_trama.set("ROBOT PARAT")
        self.label_trama.grid(row=4, column=2)     
        
        self.label_der = Label(self, text="LDR dret")
        self.label_der.grid(row=5, column=1)
        
        self.progress_bar_der = ttk.Progressbar(self, orient="horizontal",
                                        length=255, maximum=255,
                                        mode="determinate")
        self.progress_bar_der["value"] = 200
        self.progress_bar_der.place(relx=0.5, rely=0.5, relwidth=0.80,  anchor=tk.CENTER)
        self.progress_bar_der.grid(row=5, column=2)
        
        self.label_AX12_R = Label(self, text="MOTOR dret:")
        self.label_AX12_R.grid(row=6, column=1)
        self.label_motor_right = Label(self, textvariable = self.texto_motor_right)
        self.texto_motor_right.set("PARAT")
        self.label_motor_right.grid(row=6, column=2)
        
        self.quit = tk.Button(self, text="SORTIR", fg="red",
                              command=self.salir)
        self.quit.grid(row=7, column=2)

        self.logo_frame = Label(text=" ")
        self.logo_frame.grid(row = 20, column = 0, columnspan=4, pady=10)
        try:
            self.logo = PhotoImage(file="logoUB.png")           
            self.logo_frame["image"] = self.logo
        except: 
            return

    def check_queue(self):
        global instruccio
        # revisar la cola para evitar bloque en la interfaz
        if not self.cola_hilo.empty():
            # obtener mensaje de la cola
            instruccio = self.cola_hilo.get()
            if DEBUG_Consola == 1:
                print("get text from queue:", instruccio)
        if (instruccio == INSTR_STOP_THREAD):
            return INSTR_STOP_THREAD
        root.after(200, self.check_queue)
            
    def say_hi(self):
        print("J. Bosch & C. Serre,")
        print("UB, 2020.")
        messagebox.showinfo("Autors", "J. Bosch & C. Serre\nUB, 2020.")
        
    def set_debug_frames(self):
        global DEBUG_trama
        DEBUG_trama = DEBUG_trama_check.get()
        
    def set_debug_moduls(self):
        global DEBUG_Moduls
        DEBUG_Moduls = DEBUG_Moduls_check.get() 
                    
    def set_debug_consola(self):
        global DEBUG_Consola
##        print ("Debug Consola: ", DEBUG_Consola)
        DEBUG_Consola = DEBUG_Consola_check.get()
        
    def sacar_valor(self, valor): #la funcion callback del slider Scale
        kk = self.w["to"] - self.w.get()
        if DEBUG_Consola == 1:
            print(valor, kk)
        self.progress_bar_izq["value"] = valor
        self.progress_bar_der["value"] = kk

    def crear_hilo(self):
        # crear cola para comunicar/enviar tareas al thread
        self.cola = queue.Queue()
        self.cola_hilo = queue.Queue()
        if DEBUG_Consola == 1:
            print(self.cola, self.cola_hilo)
        # crear el thread 
        hilo = Hilo("puerto", self.cola,self.cola_hilo,
                    self.progress_bar_izq,self.progress_bar_der,
                    self.texto_trama, self.texto_motor_left, self.texto_motor_right)

        # iniciar thread
        hilo.start()
        
    def salir(self):
        self.cola.put(INSTR_END)# terminar funcion leer_puerto
        if(instruccio == INSTR_STOP_THREAD):
            if DEBUG_Consola == 1:
                print("Hilo finalizado")
            self.master.destroy()
        else:
            root.after(200, self.salir)
                
##root = tk.Tk()
root.title("EMULADOR ROBOT MiSE")
app = Application(master=root)
app.mainloop()
ser.close()
print("Aplicacion terminada.")
