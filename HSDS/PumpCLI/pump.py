import click
import pandas as pd
import requests
import os
import json

dpm_gid = os.getenv('DPM_GID')


@click.command()
@click.option('-o', '--organizations', type=click.Path(exists=True), help="File containing HSDS Organizations.")
@click.option('-l', '--locations', type=click.Path(exists=True), help="File containing HSDS Locations.")
@click.option('-s', '--services', type=click.Path(exists=True), help="File containing HSDS Services.")
@click.option('--url', required=True, help="The base url where the service is hosted.")
def pump_data(organizations, locations, services, url):
    """This Python script sends an HTTP PUT request where the payload is a collection of HSDS3-based entities."""

    if organizations:
        process_file('organization', organizations, url)
    if locations:
        process_file('location', locations, url)
    if services:
        process_file('service', services, url)


def process_file(choice, file, url):
    if file.endswith('.csv'):
        df = pd.read_csv(file)
    elif file.endswith('.xlsx') or file.endswith('.xls'):
        df = pd.read_excel(file)
    else:
        raise ValueError("Only CSV and Excel files are supported.")

    df['dpmgid'] = dpm_gid
    df.description = df.description.fillna(' ')  # TODO: Enforce empty description validation?
    df = df.dropna(axis=1)  # remove null columns (key, value)

    json_data = df.to_json(orient="records")

    send_to_provider_directory(url, choice, json.loads(json_data))


def send_to_provider_directory(url, choice, data):
    provider_url = url + "/hsds3/" + choice

    r = requests.put(provider_url, json=data)

    if r.status_code >= 400:
        print(r.text)
        print(r.request.body)
    r.raise_for_status()


if __name__ == '__main__':
    pump_data()
