import os
import time
import serial.tools.list_ports
import serial
import winsound
import ctypes
import sys

ser = serial.Serial()
tempfile = "C:\\QdVersaLab\\Data\\ltemp.dat"
logfile = "C:\\QdVersaLab\\Vsm\\LogFiles\\eomlog.txt"


def find_port(name="Arduino"):
    """"a funcion that finds and returns
        in wich serial port Arduino is connected """
    ports = list(serial.tools.list_ports.comports())
    for p in ports:
        if name in p.description:
            print("Arduino's port:", p[0])
            return p[0]
        else:
            continue
    error_message("ERROR: can't find Arduino")


def connect_with_arduinoo(port):
    """"this function intialise and stars
          serial comunication with arduino """
    ser.baudrate = 9600
    ser.BYTESIZES = 8
    ser.port = port
    try:
        ser.open()
    except:
        error_message("ERROR: connection with Arduino failed")
    else:
        print("connect with Arduino:", True)


def last_mod_file():
    """this func finds the las modiefied file in a folder and sub-folders"""
    path = 'C:\\QdVersaLab\\Data'
    last_file = max((os.path.join(root, f) for root, _, the_files in os.walk(path)
                     for f in the_files if f.lower().endswith(".dat")), key=os.path.getctime)
    if last_file is None:
        error_message("ERROR: cant find .dat file ")
    else:
        print("\nopen file:", last_file)
    return last_file


def log_file():
    """"this func finds the eomlog file which written by multivu when a meassurment is over.
    if log file deleted or doest exit it is created here"""
    try:
        log_file_size = os.stat(logfile).st_size
    except:
        print("file doesn't exist")
        log = open(logfile, "w+")
        log.close()
        log_file_size = os.stat(logfile).st_size
    return log_file_size


def get_field():
    """"function that read the magnetic field from arduino"""
    send = "1"
    ser.flush()
    ser.write(send.encode())
    time.sleep(0.3)
    x = ser.read(ser.inWaiting())
    x = x.decode('utf-8')
    return x


def write_to_tempfile(tempf, l, mf):
    """"function tha puts the magnetic field in the last line
             of VSM meassurments and writew the line to a tempfile"""
    t = l.split(",")
    t[44] = mf
    an = ",".join(t)
    tempf.write(an)


def copy_data(dat_file, start):
    """"function tha copies all meassurments from temp file to the data section of data file"""
    with open(dat_file, "r+") as file, open(tempfile, "r") as temp:
        file.seek(start)
        while True:
            line = temp.readline()
            if not line:
                break
            file.write(line)


def meassurment():
    """"function that sinhronizes VSM with Arduino meassurments
          by polling to data file written by VSM when it takes a meassure"""
    data_file = last_mod_file()
    log_size = log_file()
    with open(data_file, "r") as f, open(tempfile, "w") as new:
        f.seek(0, 2)
        size = f.tell()
        offset = f.tell()
        print("\n", "-" * 20, "start", "-" * 20)
        while True:
            f.seek(0, 2)
            if size != f.tell():
                field = get_field()
                f.seek(size)
                line = f.readline()
                write_to_tempfile(new, line, field)  ##############
                f.seek(0, 2)
                size = f.tell()
            elif log_size < os.stat(logfile).st_size:
                print("end of measurments\n")
                print("copy data to file")
                break
            else:
                time.sleep(0.001)
                continue
    copy_data(data_file, offset)
    os.remove(tempfile)


def find_arduino(arg):
    if len(arg) > 1:
        flag = True
        for aa in arg:
            if "COM" in aa:
                return aa
        if flag:
            name = " ".join(arg[1:])
            arduinos_port = find_port(name)
            return arduinos_port
    else:
        arduinos_port = find_port()
        return arduinos_port


def error_message(message):
    """function th prints error masseges """
    winsound.PlaySound('SystemExit', winsound.SND_ALIAS)
    print("#" * 15, message, "#" * 15)
    ctypes.windll.user32.MessageBoxW(0, message, u"Error in_vsm.exe", 0)
    exit(1)


def main(arguments):
    arduinos_port = find_arduino(arguments)
    connect_with_arduinoo(arduinos_port)
    meassurment()
    ser.close()
    exit(0)


if __name__ == '__main__':
    main(sys.argv)




