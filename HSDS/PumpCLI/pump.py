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
@click.option('--process-iterably', is_flag=True, default=False, help="Process records iterably.")
@click.option('--url', required=True, help="The base url where the service is hosted.")
def pump_data(organizations, locations, services, url, process_iterably):
    """This Python script sends an HTTP PUT request where the payload is a collection of HSDS3-based entities."""

    if organizations:
        process_file('organization', organizations, url, process_iterably)
    if locations:
        process_file('location', locations, url, process_iterably)
    if services:
        process_file('service', services, url, process_iterably)


def process_file(choice, file, url, process_iterably):
    if file.endswith('.csv'):
        df = pd.read_csv(file)
    elif file.endswith('.xlsx') or file.endswith('.xls'):
        df = pd.read_excel(file)
    else:
        raise ValueError("Only CSV and Excel files are supported.")

    df['dpmgid'] = dpm_gid
    df.description = df.description.fillna(' ')  # TODO: Enforce empty description validation?
    df = df.dropna(axis=1, how='all')  # remove null columns (key, value)

    send_to_provider_directory(url, choice, process_iterably, remove_items_if_value_is_null(df))

def remove_items_if_value_is_null(df):
    json_dict = df.to_dict(orient='records')
    for obj in json_dict:
        keys_to_remove = []
        for key, value in obj.items():
            if pd.isna(value):
                keys_to_remove.append(key)
        for key in keys_to_remove:
            obj.pop(key)

    return json_dict


def send_to_provider_directory(url, choice, process_iterably, data):
    provider_url = url + "/hsds3/" + choice

    if process_iterably:
        for obj in data:
            entity_url = provider_url + "/" + dpm_gid + "/" + obj['id']

            r = requests.put(entity_url, json=obj)

            if r.status_code >= 400:
                print("Error raised for " + choice + " record.")
                print(r.request.url)
                print(r.text)
                print(r.request.body)
                print()
    else:
        r = requests.put(provider_url, json=data)

        if r.status_code >= 400:
            print("Error raised for " + choice + " file.")
            print(r.request.url)
            print(r.text)
            print(r.request.body)
        r.raise_for_status()


if __name__ == '__main__':
    pump_data()
