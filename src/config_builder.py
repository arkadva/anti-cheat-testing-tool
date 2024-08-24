import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import yaml
import os

class AttackConfigApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Attack Configurator")

        # Set a minimum size for the window
        self.root.minsize(500, 600)

        # Configure style
        style = ttk.Style()
        style.configure("TLabel", font=("Helvetica", 12))
        style.configure("TButton", font=("Helvetica", 12))
        style.configure("TEntry", font=("Helvetica", 12))
        style.configure("TCombobox", font=("Helvetica", 12))
        style.configure("TLabelframe.Label", font=("Helvetica", 14, "bold"))

        self.attacks = [
            "ReadOffset", "ReadMemory", "WriteMemory", "ExecHeap",
            "CreateRemoteThreadInjection", "ManualMapping", "WindowsHooksInjection",
            "BreakpointHook", "SetCursorPos", "MouseEvent", "KeyboardEvent", "SendInput"
        ]

        self.attack_list = []
        self.current_attack_index = None
        self.context_entries = []

        self.create_widgets()

    def create_widgets(self):
        # Process Configuration Frame
        self.process_frame = ttk.LabelFrame(self.root, text="Process Configuration")
        self.process_frame.pack(pady=10, padx=10, fill="both", expand="yes")

        self.process_name_label = ttk.Label(self.process_frame, text="Game (process) path:")
        self.process_name_label.pack(pady=2, padx=5, anchor="w")
        self.process_name_entry = ttk.Entry(self.process_frame)
        self.process_name_entry.pack(pady=2, padx=5, fill="x")

        self.process_working_dir_label = ttk.Label(self.process_frame, text="Working Directory:")
        self.process_working_dir_label.pack(pady=2, padx=5, anchor="w")
        self.process_working_dir_entry = ttk.Entry(self.process_frame)
        self.process_working_dir_entry.pack(pady=2, padx=5, fill="x")

        self.process_args_label = ttk.Label(self.process_frame, text="Arguments (comma-separated):")
        self.process_args_label.pack(pady=2, padx=5, anchor="w")
        self.process_args_entry = ttk.Entry(self.process_frame)
        self.process_args_entry.pack(pady=2, padx=5, fill="x")

        # Dropdown menu for selecting an attack
        self.attack_label = ttk.Label(self.root, text="Select Attack:")
        self.attack_label.pack(pady=5, padx=5, anchor="w")

        self.attack_var = tk.StringVar()
        self.attack_dropdown = ttk.Combobox(self.root, textvariable=self.attack_var)
        self.attack_dropdown['values'] = self.attacks
        self.attack_dropdown.pack(pady=5, padx=10, fill="x")
        self.attack_dropdown.bind("<<ComboboxSelected>>", self.show_attack_variables)

        # Frame for inputting variables
        self.variables_frame = ttk.LabelFrame(self.root, text="Variables")
        self.variables_frame.pack(pady=10, padx=10, fill="both", expand="yes")

        # Buttons to manage attacks
        self.add_button = ttk.Button(self.root, text="Add Attack", command=self.add_attack)
        self.add_button.pack(pady=5)

        self.view_edit_button = ttk.Button(self.root, text="View/Edit Attacks", command=self.view_edit_attacks)
        self.view_edit_button.pack(pady=5)

        self.load_button = ttk.Button(self.root, text="Load Config", command=self.load_config)
        self.load_button.pack(pady=5)

        self.generate_button = ttk.Button(self.root, text="Generate YAML", command=self.generate_yaml)
        self.generate_button.pack(pady=5)

    def show_attack_variables(self, event):
        for widget in self.variables_frame.winfo_children():
            widget.destroy()

        self.clear_context_entries()

        attack = self.attack_var.get()
        variables = self.get_attack_variables(attack)

        self.var_entries = {}
        for var_name in variables:
            label = ttk.Label(self.variables_frame, text=var_name)
            label.pack(pady=2, padx=5, anchor="w")
            entry = ttk.Entry(self.variables_frame)
            entry.pack(pady=2, padx=5, fill="x")
            self.var_entries[var_name] = entry

        if attack == "BreakpointHook":
            self.add_context_management()

    def get_attack_variables(self, attack):
        attack_vars = {
            "ReadOffset": ["address", "save_to", "offsets", "method"],
            "ReadMemory": ["address", "save_to", "type", "method"],
            "WriteMemory": ["address", "value", "type", "method"],
            "ExecHeap": ["shellcode"],
            "CreateRemoteThreadInjection": ["path"],
            "ManualMapping": ["path"],
            "WindowsHooksInjection": ["path"],
            "BreakpointHook": ["address", "type"],  # 'context' will be added dynamically
            "SetCursorPos": ["cursor_x", "cursor_y"],
            "MouseEvent": ["event"],
            "KeyboardEvent": ["event", "key"],
            "SendInput": ["type", "event", "key"]
        }
        return attack_vars.get(attack, [])

    def add_context_management(self):
        context_label = ttk.Label(self.variables_frame, text="Context")
        context_label.pack(pady=2, padx=5, anchor="w")

        add_context_button = ttk.Button(self.variables_frame, text="Add Context", command=self.add_context_entry)
        add_context_button.pack(pady=2, padx=5)

    def add_context_entry(self):
        context_frame = ttk.Frame(self.variables_frame)
        context_frame.pack(fill="x", pady=5)

        register_label = ttk.Label(context_frame, text="Register:")
        register_label.pack(side="left", padx=5)

        register_entry = ttk.Entry(context_frame)
        register_entry.pack(side="left", padx=5, fill="x", expand=True)
        
        value_label = ttk.Label(context_frame, text="Value:")
        value_label.pack(side="left", padx=5)

        value_entry = ttk.Entry(context_frame)
        value_entry.pack(side="left", padx=5, fill="x", expand=True)

        remove_button = ttk.Button(context_frame, text="Remove", command=lambda: self.remove_context_entry(context_frame))
        remove_button.pack(side="left", padx=5)

        self.context_entries.append((register_entry, value_entry))

    def remove_context_entry(self, context_frame):
        context_frame.destroy()
        self.context_entries = [(reg, val) for reg, val in self.context_entries if reg.winfo_exists() and val.winfo_exists()]

    def add_attack(self):
        if not self.attack_var.get():
            messagebox.showwarning("Input Error", "Please select an attack first.")
            return

        if not self.var_entries:
            messagebox.showwarning("Input Error", "Please fill in all variables before adding.")
            return

        variables = {var: entry.get() for var, entry in self.var_entries.items()}
        context = [{"register": reg.get(), "value": val.get()} for reg, val in self.context_entries if reg.get() and val.get()]

        if self.attack_var.get() == "BreakpointHook":
            variables["context"] = context

        attack = {"name": self.attack_var.get(), "variables": variables}

        if self.current_attack_index is not None:
            self.attack_list[self.current_attack_index] = attack
            self.current_attack_index = None
        else:
            self.attack_list.append(attack)

        messagebox.showinfo("Success", "Attack added successfully!")
        self.clear_variables_frame()

    def clear_variables_frame(self):
        for widget in self.variables_frame.winfo_children():
            widget.destroy()
        self.attack_var.set('')

    def clear_context_entries(self):
        for reg, val in self.context_entries:
            reg.destroy()
            val.destroy()
        self.context_entries = []

    def view_edit_attacks(self):
        top = tk.Toplevel(self.root)
        top.title("View/Edit Attacks")

        self.attack_listbox = tk.Listbox(top, width=50, height=15)
        self.attack_listbox.pack(pady=10, padx=10)

        for attack in self.attack_list:
            self.attack_listbox.insert(tk.END, f"{attack['name']} - {attack['variables']}")

        edit_button = ttk.Button(top, text="Edit Selected", command=self.edit_selected_attack)
        edit_button.pack(pady=5)

    def edit_selected_attack(self):
        selected_index = self.attack_listbox.curselection()
        if not selected_index:
            messagebox.showwarning("Selection Error", "Please select an attack to edit.")
            return

        selected_index = selected_index[0]
        self.current_attack_index = selected_index
        attack = self.attack_list[selected_index]

        self.attack_var.set(attack['name'])
        self.show_attack_variables(None)  # Rebuild variable fields

        for var_name, value in attack['variables'].items():
            if var_name == "context":
                for context in value:
                    self.add_context_entry()
                    self.context_entries[-1][0].insert(0, context['register'])
                    self.context_entries[-1][1].insert(0, context['value'])
            else:
                if var_name in self.var_entries:
                    self.var_entries[var_name].delete(0, tk.END)
                    self.var_entries[var_name].insert(0, value)


    def generate_yaml(self):
        process = {
            "name": self.process_name_entry.get(),
            "working_directory": self.process_working_dir_entry.get(),
            "args": [arg.strip() for arg in self.process_args_entry.get().split(",")]
        }

        config = {
            "process": process,
            "attacks": self.attack_list
        }

        config_path = "config.yaml"
        backup_path = "config_old.yaml"

        # Check if the config file already exists and rename it if it does
        if os.path.exists(config_path):
            os.rename(config_path, backup_path)

        with open(config_path, "w") as file:
            yaml.safe_dump(config, file, default_flow_style=False, allow_unicode=True)

        messagebox.showinfo("Success", "YAML file generated successfully!")

    def load_config(self):
        file_path = filedialog.askopenfilename(filetypes=[("YAML files", "*.yaml"), ("All files", "*.*")])
        if not file_path:
            return

        with open(file_path, "r") as file:
            config = yaml.safe_load(file)

        process = config.get("process", {})
        self.process_name_entry.delete(0, tk.END)
        self.process_name_entry.insert(0, process.get("name", ""))

        self.process_working_dir_entry.delete(0, tk.END)
        self.process_working_dir_entry.insert(0, process.get("working_directory", ""))

        self.process_args_entry.delete(0, tk.END)
        self.process_args_entry.insert(0, ", ".join(process.get("args", [])))

        self.attack_list = config.get("attacks", [])
        messagebox.showinfo("Success", "Configuration loaded successfully!")

if __name__ == "__main__":
    root = tk.Tk()
    app = AttackConfigApp(root)
    root.mainloop()
