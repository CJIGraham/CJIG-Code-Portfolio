import winsound as w
import time as t
import json

#Standard Morse Code Timings - All timings sourced from https://en.wikipedia.org/wiki/Morse_code#Representation,_timing,_and_speeds
interElementGap = .1
shortGap = .3
mediumGap = .7

#Frequency of sound output, in Hz
frequency = 500

#Hard-Coded morse code for each letter to save processing time during output. (100 is dit, 300 is dah, time measured in milliseconds).
letters = '{"A":[100, 300], "B":[300, 100, 100, 100], "C":[300, 100, 300, 100], "D":[300, 100, 100], "E":[100], "F":[100, 100, 300, 100], "G":[300, 300, 100], "H":[100, 100, 100, 100], "I":[300, 100, 100, 100], "J":[100, 100], "K":[300, 100, 300], "L":[100, 300, 100, 100], "M":[300, 300], "N":[300, 100], "O":[300, 300, 300], "P":[100, 300, 300, 100], "Q":[300, 300, 100, 300], "R":[100, 300, 100], "S":[100, 100, 100], "T":[300], "U":[100, 100, 300], "V":[100, 100, 100, 300], "W":[100, 300, 300], "X":[300, 100, 100, 300], "Y":[300, 100, 300, 300], "Z":[300, 300, 100, 100]}'

#Validation to only accept letters in user input.
while True:
    sentence = input("Enter the sentence that you would like to have translated: ")
    if sentence.isalpha():
        break
    print("Sentence must only contain characters A-Z.")

tones = json.loads(letters)
sentence = sentence.upper()
for letter in sentence:
    if letter == ' ':
        t.sleep(mediumGap) #Gap between words
    else: 
        print("Outputting letter: " + letter + " : " + str(tones[letter]))
        for i in tones[letter]:
            w.Beep(frequency, i)
            t.sleep(interElementGap) #Gap between dits/dahs within letter
        t.sleep(shortGap) #Gap between letters