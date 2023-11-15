import requests

url = "https://humor-jokes-and-memes.p.rapidapi.com/jokes/search"

querystring = {"exclude-tags":"nsfw","keywords":"rocket","min-rating":"7","include-tags":"one_liner","number":"3","max-length":"200"}

headers = {
	"X-RapidAPI-Key": "0eaacbba04msh33954e21aeb722dp12946fjsna2b292f5ccd8",
	"X-RapidAPI-Host": "humor-jokes-and-memes.p.rapidapi.com"
}

response = requests.get(url, headers=headers, params=querystring)

print(response.json())