cd /u/prj/wella/exe
sleep 1; ./display -s 1 -i 1 -n 0 --display=dsp11:0.0 -m &
sleep 1; ./display -s 2 -i 1 -n 1 --display=dsp12:0.0 -m &
sleep 1; ./display -s 3 -i 1 -n 2 --display=dsp13:0.0 -m &
sleep 1; ./display -s 4 -i 1 -n 3 --display=dsp14:0.0 -m &
sleep 1; ./display -s 5 -i 1 -n 4 --display=dsp15:0.0 -m &
sleep 1; ./display -s 6 -i 1 -n 5 --display=dsp16:0.0 -m &
sleep 1; ./display -s 7 -i 1 -n 6 --display=dsp17:0.0 -m &
sleep 1; ./display -s 8 -i 1 -n 7 --display=dsp18:0.0 -m &

# dopo varie sostituzioni questa e' la situazione: 
# Settore 1 -> Display 3
# Settore 2 -> Display 1
# Settore 3 -> Display 2

sleep 1; ./display -s 1 -i 2 -n 8 --display=dsp23:0.0 -m &
sleep 1; ./display -s 2 -i 2 -n 9 --display=dsp21:0.0 -m &
sleep 1; ./display -s 3 -i 2 -n 10 --display=dsp22:0.0 -m &
sleep 1; ./display -s 4 -i 2 -n 11 --display=dsp24:0.0 -m &
sleep 1; ./display -s 5 -i 2 -n 12 --display=dsp25:0.0 -m &
sleep 1; ./display -s 6 -i 2 -n 13 --display=dsp26:0.0 -m &
sleep 1; ./display -s 7 -i 2 -n 14 --display=dsp27:0.0 -m &
sleep 1; ./display -s 8 -i 2 -n 15 --display=dsp28:0.0 -m &