import requests
from bs4 import BeautifulSoup

#Request HTML from AWD-IT's Nvidia Geforce RTX 4090 webpage
URL = "https://www.awd-it.co.uk/components/graphics-cards/nvidia/nvidia-geforce-rtx-4090.html"
page = requests.get(URL).text
soup = BeautifulSoup(page, 'html.parser')

prices = []

#Find all products on the page
products = soup.find_all("li", {"class": "item product product-item"})
#Loop through all the products, if they have a price available add it to the prices array.
#(If no price is available it will error, this is handled with the try/except)
for i in range(0,len(products)):
    try:
        product = [products[i].find("a", {"class": "product-item-link"}).text.strip(), products[i].find("span", {"class": "price"}).text]
        prices.append(product)
    except:
        pass

#Display all products with prices
for price in prices:
    print(price[0] + ": " + price[1])
