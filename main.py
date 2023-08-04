import pygame
from pygame.locals import *
import sys
import serial
import time
import cv2
import numpy as np

mega = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
uno = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(1)

pygame.init()
cam = cv2.VideoCapture(0)
pygame.display.set_caption('USV')

res = (1920,1080)  
screen = pygame.display.set_mode(res)
width = screen.get_width()
height = screen.get_height()

color_white = (255,255,255)
color_black = (51,0,0)
color_light = (170,170,170) 
color_dark = (100,100,100)
color_blue = (0,0,255)
  
normalfont = pygame.font.SysFont('arial',30)
smallfont = pygame.font.SysFont('arial',15)
logo_font = pygame.font.SysFont('Rubik',100)

def draw_text(text, font, text_col, x, y):
    img = normalfont.render(text, True, text_col)
    screen.blit(img, (x, y))

depth_data = ''
home_distance = ''
next_waypoint = ''
compass_data = ''
position1 = ''
position2 = ''
compass = ''

a = [1,2,3,4,5,6,7,8,9,10,11,12]

file1 = open('data.txt', 'r')
Lines = file1.readlines()

count = 0
for line in Lines:
    count += 1
    a[count] = line.strip()


while True:
      
    for ev in pygame.event.get():
          
        if ev.type == pygame.QUIT:
            pygame.quit()
        
        if ev.type == pygame.KEYDOWN:
            if ev.key == pygame.K_RETURN:
                mega.write(bytes('1', 'utf-8'))
                mega.write(bytes('1', 'utf-8'))
                mega.write(bytes('S', 'utf-8'))
                mega.write(bytes('S', 'utf-8'))
            elif ev.key == pygame.K_SPACE:
                mega.write(bytes('2', 'utf-8'))
                mega.write(bytes('2', 'utf-8'))
                mega.write(bytes('S', 'utf-8'))
                mega.write(bytes('S', 'utf-8'))                

              
    screen.fill((255, 195, 0))
      
    mouse = pygame.mouse.get_pos()


    # Logo
    image_logo= pygame.image.load("boat.png")
    image_logo= pygame.transform.scale(image_logo, (330,250))
    screen.blit(image_logo, (width/4,height/9))

    screen.blit(logo_font.render('TÀU ĐA NĂNG' , True , color_black) , (width/2.4,height/5))

    # Background Left
    image_left3 = pygame.image.load("light_yellow.png")
    image_left3= pygame.transform.scale(image_left3, (400,500))
    screen.blit(image_left3, (width/12-30,height/1.55))

    # Background Right
    image_left = pygame.image.load("light_yellow.png")
    image_left = pygame.transform.scale(image_left,(2000,3000))
    screen.blit(image_left, (width/1.4,height/3+25))

    image_left2 = pygame.image.load("light_yellow.png")
    image_left2 = pygame.transform.scale(image_left2,(2000,100))
    screen.blit(image_left2, (width/1.4,height/7))


    # Get
    if width/15 <= mouse[0] <= width/15+400 and height/10 <= mouse[1] <= height/10+100:
        pygame.draw.rect(screen,color_light,[width/15,height/10,400,100])
    else:
        pygame.draw.rect(screen,color_white,[width/15,height/10,400,100])

    screen.blit(normalfont.render('GET WATER' , True , color_black) , (width/8,height/10+40))

    # Stop
    if width/15 <= mouse[0] <= width/15+400 and height/4 <= mouse[1] <= height/4+100:
        pygame.draw.rect(screen,color_light,[width/15,height/4,400,100])
    else:
        pygame.draw.rect(screen,color_white,[width/15,height/4,400,100])

    screen.blit(normalfont.render('STOP' , True , color_black) , (width/6-30,height/3.5))    

    # Control
    screen.blit(normalfont.render('Manual Control' , True , color_black) , (width/9+30,height/2.5))
    screen.blit(normalfont.render('←' , True , color_black) , (width/7.25,height/2))
    screen.blit(normalfont.render('↑' , True , color_black) , (width/6,height/2.2))
    screen.blit(normalfont.render('→' , True , color_black) , (width/6+50,height/2))
    screen.blit(normalfont.render('↓' , True , color_black) , (width/6,height/1.8))

    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        mega.write(bytes('L', 'utf-8'))
        mega.write(bytes('L', 'utf-8')) 
    if keys[pygame.K_RIGHT]:
        mega.write(bytes('R', 'utf-8'))
        mega.write(bytes('R', 'utf-8')) 
    if keys[pygame.K_UP]:
        mega.write(bytes('F', 'utf-8'))
        mega.write(bytes('F', 'utf-8'))   
    if keys[pygame.K_DOWN]:
        mega.write(bytes('B', 'utf-8'))
        mega.write(bytes('B', 'utf-8')) 



    data = mega.readline().decode('utf-8').rstrip()
    data2 = uno.readline().decode('utf-8').rstrip()
    time.sleep(0.01)

    #Depth
    screen.blit(normalfont.render('Depth(cm)' , True , color_black) , (width/9+50,height/1.4)) 
    if data2[0] == 'D':
        depth_data = data2[1:]

    draw_text(depth_data, normalfont, color_black, width/8+60, height/1.2)

    #Home location
    image_home = pygame.image.load("home.png")
    image_home = pygame.transform.scale(image_home,(30,30))
    screen.blit(image_home, (width/1.35,height/6))
    screen.blit(normalfont.render('Home location' , True , color_black) , (width/1.3,height/6))
    screen.blit(smallfont.render('16.098109   20.102109' , True , color_blue) , (width/1.3,height/4.75)) 

    #To home
    image_next = pygame.image.load("distance.png")
    image_next = pygame.transform.scale(image_next,(30,30))
    screen.blit(image_next, (width/1.35,height/4))
    screen.blit(normalfont.render('To home:' , True , color_black) , (width/1.3,height/4)) 
    if data[0] == 'H':
        home_distance = data[1:]

    draw_text(home_distance+' m', normalfont, color_black, width/1.15, height/4)

    #Next waypoint
    screen.blit(normalfont.render('Next waypoint:' , True , color_black) , (width/1.35,height/3.25)) 
    if data[0] == 'W':
        next_waypoint = data[1:]

    draw_text(next_waypoint+' m', normalfont, color_black, width/1.15, height/3.25)

    #Compass

    image_compass = pygame.image.load("compass.png")
    image_compass = pygame.transform.scale(image_compass,(250,250))
    screen.blit(image_compass, (width/1.3,height/2.75))
    
    if data2[0] == 'C':
        compass_data = data2[1:]
        compass = 'C'+compass_data+'\n'
      
    mega.write(compass.encode('utf-8'))
 
    draw_text(compass_data+'°', normalfont, color_blue, width/1.25+20, height/2.3)
    screen.blit(normalfont.render('Compass' , True , color_black) , (width/1.25,height/2)) 

    #Camera
    ret, frame = cam.read()
    frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame = np.rot90(frame)
    frame = pygame.surfarray.make_surface(frame)
    frame= pygame.transform.scale(frame, (800,700))
    screen.blit(frame, (width/3.5,height/2.5))

    #GPS
    image_gps = pygame.image.load("gps.png")
    image_gps = pygame.transform.scale(image_gps,(25,25))
    screen.blit(image_gps, (width/1.38,height/1.6))
    screen.blit(normalfont.render('Position:' , True , color_black) , (width/1.35,height/1.6)) 
    if data[0] == 'P':
        position1 = data[1:]
    elif data[0] == 'K':
        position2 = data[1:]


    draw_text(position1+'   '+position2, normalfont, color_blue, width/1.2, height/1.6)
    draw_text('Waypoint 1: '+a[1]+'  '+a[2], normalfont, color_blue, width/1.4+25,height/1.25-140)
    draw_text('Waypoint 2: '+a[3]+'  '+a[4], normalfont, color_blue, width/1.4+25,height/1.25-90)
    draw_text('Waypoint 3: '+a[5]+'  '+a[6], normalfont, color_blue, width/1.4+25,height/1.25-40)
    draw_text('Waypoint 4: '+a[7]+'  '+a[8], normalfont, color_blue, width/1.4+25,height/1.25+10)
    draw_text('Waypoint 5: '+a[9]+'  '+a[10], normalfont, color_blue, width/1.4+25,height/1.25+60) 


    pygame.display.update()
