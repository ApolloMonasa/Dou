import sys
import json
import subprocess
import os
import tkinter as tk
from tkinter import ttk, messagebox
import threading

# Force explicit Tcl/Tk paths for PyInstaller environment
if hasattr(sys, '_MEIPASS'):
    os.environ['TCL_LIBRARY'] = os.path.join(sys._MEIPASS, 'tcl8.6')
    os.environ['TK_LIBRARY'] = os.path.join(sys._MEIPASS, 'tk8.6')

# C++ Solver Path
SOLVER_PATH = "dou_solver.exe"

class SolverThread:
    def __init__(self, hand_a, hand_b, callback, error_callback):
        self.hand_a = hand_a
        self.hand_b = hand_b
        self.callback = callback
        self.error_callback = error_callback
        self.process = None
        self.running = True
        self.thread = threading.Thread(target=self.run)
        self.thread.daemon = True

    def start(self):
        self.thread.start()

    def run(self):
        try:
            input_str = self.format_input(self.hand_a, self.hand_b)
            with open("input.txt", "w") as f:
                f.write(input_str)

            creationflags = 0
            if os.name == 'nt':
                creationflags = subprocess.CREATE_NO_WINDOW

            self.process = subprocess.Popen(
                [SOLVER_PATH, "--json"],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                bufsize=0,
                creationflags=creationflags
            )

            self.read_output()

        except Exception as e:
            self.error_callback(str(e))

    def format_input(self, hand_a, hand_b):
        def line(hand):
            return " ".join(map(str, hand)) + " 0"
        return f"{line(hand_a)}\n{line(hand_b)}\n"

    def send_input(self, index):
        if self.process and self.process.poll() is None:
            try:
                self.process.stdin.write(f"{index}\n")
                self.process.stdin.flush()
                # Reading happens in the run loop
            except Exception as e:
                self.error_callback(str(e))

    def read_output(self):
        if not self.process:
            return
            
        while self.running:
            line = self.process.stdout.readline()
            if not line:
                break
            line = line.strip()
            if not line:
                continue
            
            if line.startswith("{"):
                try:
                    data = json.loads(line)
                    self.callback(data)
                except json.JSONDecodeError:
                    print(f"JSON Error: {line}")
            else:
                print(f"Solver Log: {line}")

    def stop(self):
        self.running = False
        if self.process:
            self.process.terminate()

class CardSelectorDialog(tk.Toplevel):
    def __init__(self, parent, initial_hand_a, initial_hand_b):
        super().__init__(parent)
        self.title("配置手牌 (Setup Hands)")
        self.geometry("800x600")
        
        # Copy hands to avoid modifying original until confirmed
        self.hand_a = sorted(initial_hand_a)
        self.hand_b = sorted(initial_hand_b)
        self.current_target = tk.StringVar(value="Me") # "Me" or "Opp"
        
        # Global card pool (full deck)
        # 3-15 (3-2), 16 (Small Joker), 17 (Big Joker)
        self.deck = {i: 4 for i in range(3, 16)}
        self.deck[16] = 1
        self.deck[17] = 1
        
        # Calculate used cards
        self._recalc_deck()
        
        self.setup_ui()
        self.refresh_ui()

    def _recalc_deck(self):
        # Reset to full deck
        self.deck = {i: 4 for i in range(3, 16)}
        self.deck[16] = 1
        self.deck[17] = 1
        
        # Subtract used cards
        for c in self.hand_a: self.deck[c] -= 1
        for c in self.hand_b: self.deck[c] -= 1

    def setup_ui(self):
        main_frame = ttk.Frame(self, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 1. Target Selection
        target_frame = ttk.LabelFrame(main_frame, text="当前操作对象 (Select Target)")
        target_frame.pack(fill=tk.X, pady=5)
        
        ttk.Radiobutton(target_frame, text="我方 (Me)", variable=self.current_target, value="Me").pack(side=tk.LEFT, padx=20, pady=5)
        ttk.Radiobutton(target_frame, text="对手 (Opponent)", variable=self.current_target, value="Opp").pack(side=tk.LEFT, padx=20, pady=5)
        
        # 2. Card Pool (Click to ADD)
        pool_frame = ttk.LabelFrame(main_frame, text="点击添加手牌 (Card Pool)")
        pool_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        self.pool_buttons = {}
        # Layout grid: 3-9, 10-A, 2-Jokers
        row1 = list(range(3, 10))
        row2 = list(range(10, 15))
        row3 = [15, 16, 17]
        
        for r_idx, row_cards in enumerate([row1, row2, row3]):
            row_frame = ttk.Frame(pool_frame)
            row_frame.pack(fill=tk.X, pady=2)
            for val in row_cards:
                name = self.get_card_name(val)
                btn = tk.Button(row_frame, text=f"{name}\n(4)", width=6, height=3,
                                command=lambda v=val: self.add_card(v))
                btn.pack(side=tk.LEFT, padx=5)
                self.pool_buttons[val] = btn

        # 3. Preview Areas (Click to REMOVE)
        preview_frame = ttk.Frame(main_frame)
        preview_frame.pack(fill=tk.BOTH, expand=True, pady=5)
        
        # Me
        me_frame = ttk.LabelFrame(preview_frame, text="我方手牌 (Me) - 点击删除")
        me_frame.pack(fill=tk.X, pady=5)
        self.me_container = ttk.Frame(me_frame)
        self.me_container.pack(fill=tk.X)
        
        # Opp
        opp_frame = ttk.LabelFrame(preview_frame, text="对手手牌 (Opponent) - 点击删除")
        opp_frame.pack(fill=tk.X, pady=5)
        self.opp_container = ttk.Frame(opp_frame)
        self.opp_container.pack(fill=tk.X)
        
        # 4. Actions
        action_frame = ttk.Frame(main_frame)
        action_frame.pack(fill=tk.X, pady=10)
        
        ttk.Button(action_frame, text="清空 (Clear All)", command=self.clear_all).pack(side=tk.LEFT, padx=5)
        ttk.Button(action_frame, text="确认 (Confirm)", command=self.on_confirm).pack(side=tk.RIGHT, padx=5)
        ttk.Button(action_frame, text="取消 (Cancel)", command=self.destroy).pack(side=tk.RIGHT, padx=5)

    def get_card_name(self, v):
        if v < 11: return str(v)
        if v == 11: return "J"
        if v == 12: return "Q"
        if v == 13: return "K"
        if v == 14: return "A"
        if v == 15: return "2"
        if v == 16: return "小王"
        if v == 17: return "大王"
        return "?"

    def add_card(self, val):
        if self.deck[val] > 0:
            if self.current_target.get() == "Me":
                self.hand_a.append(val)
                self.hand_a.sort()
            else:
                self.hand_b.append(val)
                self.hand_b.sort()
            self._recalc_deck()
            self.refresh_ui()

    def remove_card(self, val, owner):
        if owner == "Me":
            self.hand_a.remove(val)
        else:
            self.hand_b.remove(val)
        self._recalc_deck()
        self.refresh_ui()

    def clear_all(self):
        self.hand_a = []
        self.hand_b = []
        self._recalc_deck()
        self.refresh_ui()

    def refresh_ui(self):
        # Update Pool Buttons
        for val, btn in self.pool_buttons.items():
            count = self.deck[val]
            name = self.get_card_name(val)
            btn.config(text=f"{name}\n({count})", state=tk.NORMAL if count > 0 else tk.DISABLED)
            
        # Update Hand Previews
        for widget in self.me_container.winfo_children(): widget.destroy()
        for widget in self.opp_container.winfo_children(): widget.destroy()
        
        def draw_hand(container, hand, owner):
            for val in hand:
                name = self.get_card_name(val)
                color = "red" if val >= 16 else "black"
                btn = tk.Button(container, text=name, font=("Arial", 10, "bold"), fg=color,
                                bg="#ffffff", width=3,
                                command=lambda v=val, o=owner: self.remove_card(v, o))
                btn.pack(side=tk.LEFT, padx=1)
                
        draw_hand(self.me_container, self.hand_a, "Me")
        draw_hand(self.opp_container, self.hand_b, "Opp")

    def on_confirm(self):
        # Check for app_instance attribute
        if hasattr(self, 'app_instance'):
            self.app_instance.update_hands(self.hand_a, self.hand_b)
        # Fallback to master if it has the method (old way)
        elif hasattr(self.master, 'update_hands'):
            self.master.update_hands(self.hand_a, self.hand_b)
            
        self.destroy()

class NeoDouApp:
    def __init__(self, root):
        self.root = root
        self.root.title("NeoDou Solver GUI (Tkinter)")
        self.root.geometry("900x700")
        
        self.solver = None
        self.hand_a = []
        self.hand_b = []
        
        self.setup_ui()
        self.load_initial_hands()

    def setup_ui(self):
        # Styles
        style = ttk.Style()
        style.configure("Card.TButton", font=("Arial", 12, "bold"), width=4, padding=5)
        style.configure("Good.TButton", background="#d0f0c0")
        style.configure("Bad.TButton", background="#f0d0d0")
        
        # Main Layout
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.pack(fill=tk.BOTH, expand=True)
        
        # 1. Opponent Hand
        ttk.Label(main_frame, text="对手手牌 (Opponent):", font=("Arial", 12)).pack(anchor=tk.W)
        self.opp_frame = ttk.Frame(main_frame)
        self.opp_frame.pack(fill=tk.X, pady=5)
        
        # 2. Status
        self.status_var = tk.StringVar(value="请点击'开始计算'启动求解器")
        self.status_label = ttk.Label(main_frame, textvariable=self.status_var, font=("Arial", 14), foreground="blue")
        self.status_label.pack(pady=10)
        
        # 3. My Hand
        ttk.Label(main_frame, text="我方手牌 (Me):", font=("Arial", 12)).pack(anchor=tk.W)
        self.my_frame = ttk.Frame(main_frame)
        self.my_frame.pack(fill=tk.X, pady=5)
        
        # 4. Controls
        controls = ttk.Frame(main_frame)
        controls.pack(pady=10)
        
        self.btn_reset = ttk.Button(controls, text="重新开始 (Reset)", command=self.load_initial_hands)
        self.btn_reset.pack(side=tk.LEFT, padx=5)
        
        self.btn_setup = ttk.Button(controls, text="配置牌型 (Setup)", command=self.open_setup_dialog)
        self.btn_setup.pack(side=tk.LEFT, padx=5)
        
        self.btn_start = ttk.Button(controls, text="开始计算 (Start Solver)", command=self.start_solver)
        self.btn_start.pack(side=tk.LEFT, padx=5)
        
        # 5. Options
        ttk.Label(main_frame, text="推荐出牌 (Options):", font=("Arial", 12)).pack(anchor=tk.W)
        
        # Canvas for scrollable options
        self.canvas = tk.Canvas(main_frame)
        self.scrollbar = ttk.Scrollbar(main_frame, orient="vertical", command=self.canvas.yview)
        self.options_frame = ttk.Frame(self.canvas)
        
        self.options_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )
        
        self.canvas.create_window((0, 0), window=self.options_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=self.scrollbar.set)
        
        self.canvas.pack(side="left", fill="both", expand=True)
        self.scrollbar.pack(side="right", fill="y")

    def get_card_name(self, v):
        if v < 11: return str(v)
        if v == 11: return "J"
        if v == 12: return "Q"
        if v == 13: return "K"
        if v == 14: return "A"
        if v == 15: return "2"
        if v == 16: return "小王"
        if v == 17: return "大王"
        return "?"

    def open_setup_dialog(self):
        try:
            # Pass self.root as parent to ensure correct stacking
            dlg = CardSelectorDialog(self.root, self.hand_a, self.hand_b)
            # We need to manually link the callback since we can't pass 'self' as master if we want self.root as parent widget
            # Actually, passing self.root is better for Toplevel.
            # But CardSelectorDialog expects 'parent' to have 'update_hands' method if we use confirm logic as implemented.
            # Let's modify CardSelectorDialog to accept a callback or store reference properly.
            
            # Quick fix: Pass 'self' (the app instance) as a custom attribute
            dlg.app_instance = self
            
            # Force focus
            dlg.lift()
            dlg.focus_force()
            dlg.grab_set() # Modal dialog
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to open setup dialog: {str(e)}")

    def update_hands(self, hand_a, hand_b):
        self.hand_a = hand_a
        self.hand_b = hand_b
        self.refresh_hands()
        self.status_var.set("手牌已更新，请点击'开始计算'")
        self.btn_start.config(state=tk.NORMAL)
        self.clear_options()

    def load_initial_hands(self):
        self.status_var.set("请点击'开始计算'启动求解器")
        self.btn_start.config(state=tk.NORMAL)
        
        # Default hands
        self.hand_a = [3,3,3,4,4,4,5,5,6,6]
        self.hand_b = [7]
        
        if os.path.exists("input.txt"):
            try:
                with open("input.txt", "r") as f:
                    lines = f.readlines()
                    if len(lines) >= 2:
                        self.hand_a = [int(x) for x in lines[0].strip().split() if x != '0']
                        self.hand_b = [int(x) for x in lines[1].strip().split() if x != '0']
            except:
                pass
        
        self.refresh_hands()
        self.clear_options()

    def refresh_hands(self):
        # Clear frames
        for widget in self.my_frame.winfo_children(): widget.destroy()
        for widget in self.opp_frame.winfo_children(): widget.destroy()
        
        # Draw My Hand
        self.hand_a.sort()
        for val in self.hand_a:
            name = self.get_card_name(val)
            color = "red" if val >= 16 else "black"
            lbl = tk.Label(self.my_frame, text=name, font=("Arial", 14, "bold"), fg=color, 
                           bg="#e0e0e0", width=4, height=2, relief="raised")
            lbl.pack(side=tk.LEFT, padx=2)
            
        # Draw Opp Hand
        self.hand_b.sort()
        for val in self.hand_b:
            name = self.get_card_name(val)
            color = "red" if val >= 16 else "black"
            lbl = tk.Label(self.opp_frame, text=name, font=("Arial", 14, "bold"), fg=color,
                           bg="#e0e0e0", width=4, height=2, relief="raised")
            lbl.pack(side=tk.LEFT, padx=2)

    def start_solver(self):
        if self.solver:
            self.solver.stop()
        
        self.solver = SolverThread(self.hand_a, self.hand_b, self.on_solver_update, self.on_solver_error)
        self.solver.start()
        
        self.btn_start.config(state=tk.DISABLED)
        self.status_var.set("正在计算...")

    def on_solver_update(self, data):
        # Use root.after to update UI from main thread
        self.root.after(0, lambda: self._update_ui(data))

    def _update_ui(self, data):
        if "hand_a" in data:
            self.hand_a = data["hand_a"]
        if "hand_b" in data:
            self.hand_b = data["hand_b"]
            
        self.refresh_hands()
        
        turn = data.get("turn", "")
        winner = data.get("winner")
        
        if winner:
            self.status_var.set(f"游戏结束！获胜者: {winner}")
            self.clear_options()
            return
            
        if turn == "A":
            self.status_var.set("轮到我方出牌 (My Turn)")
        else:
            self.status_var.set("轮到对手出牌 (Opponent Turn) - 请等待或手动选择")
            
        self.update_options(data.get("options", []))

    def update_options(self, options):
        self.clear_options()
        for opt in options:
            prefix = "[必胜]" if not opt['win'] else "[必败]"
            text = f"{prefix} {opt['desc']}"
            bg = "#d0f0c0" if not opt['win'] else "#f0d0d0"
            
            btn = tk.Button(self.options_frame, text=text, bg=bg, font=("Arial", 11),
                            command=lambda idx=opt['id']: self.make_move(idx),
                            anchor="w", justify="left", padx=10)
            btn.pack(fill=tk.X, pady=2, padx=5)

    def clear_options(self):
        for widget in self.options_frame.winfo_children():
            widget.destroy()

    def make_move(self, index):
        if self.solver:
            self.solver.send_input(index)

    def on_solver_error(self, msg):
        self.root.after(0, lambda: messagebox.showerror("Error", msg))
        self.root.after(0, lambda: self.btn_start.config(state=tk.NORMAL))

    def on_close(self):
        if self.solver:
            self.solver.stop()
        self.root.destroy()

if __name__ == "__main__":
    root = tk.Tk()
    app = NeoDouApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_close)
    root.mainloop()
