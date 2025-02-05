from flask import Flask, request, jsonify, render_template
import requests

app = Flask(__name__)

OLLAMA_URL = "http://localhost:11434/api/generate"  # Change if needed

@app.route("/")
def home():
    return render_template("./index.html")  # Serve the chat UI

@app.route("/chat", methods=["POST"])
def chat():
    user_message = request.json.get("message", "")
    
    if not user_message:
        return jsonify({"error": "Empty message"}), 400

    # Prepare request for Ollama
    ollama_request = {
        "model": "deepseek-r1:1.5b",
        "prompt": user_message,
        "stream": False  # Get full response at once
    }

    # Send request to Ollama
    try:
        response = requests.post(OLLAMA_URL, json=ollama_request)
        response_json = response.json()
        full_response = response_json.get("response", "No response received.")
    except Exception as e:
        return jsonify({"error": str(e)}), 500

    return jsonify({"response": full_response})

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=True)  # Accessible to all on the network
